// Copyright (c) 2011-2015 The Bitcoin Core developers
// Copyright (c) 2018-2020 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

//
// Unit tests for block-chain checkpoints
//

#include <checkpoints.h>

#include <chain.h>
#include <chainparams.h>
#include <config.h>
#include <consensus/validation.h>
#include <pow.h>
#include <streams.h>
#include <uint256.h>
#include <util/strencodings.h>
#include <validation.h>

#include <test/setup_common.h>

#include <boost/test/unit_test.hpp>

#include <memory>

BOOST_FIXTURE_TEST_SUITE(checkpoints_tests, TestingSetup)

BOOST_AUTO_TEST_CASE(sanity) {
    const auto params = CreateChainParams(CBaseChainParams::MAIN);
    const CCheckpointData &checkpoints = params->Checkpoints();

    const BlockHash genesis = BlockHash::fromHex(
        "000000003ab967d9379a450afdb1f7e849434ff7bc48d99a4efef91be531ae55");

    const BlockHash wrong = BlockHash::fromHex(
        "0000000000000000000000000000000000000000000000000000000000000001");

    // NVU mainnet checkpoints its unique genesis block.
    BOOST_CHECK(Checkpoints::CheckBlock(checkpoints, 0, genesis));

    // A different block hash at the genesis checkpoint must fail.
    BOOST_CHECK(!Checkpoints::CheckBlock(checkpoints, 0, wrong));

    // Heights without checkpoints accept arbitrary block hashes.
    BOOST_CHECK(Checkpoints::CheckBlock(checkpoints, 1, wrong));
    BOOST_CHECK(Checkpoints::CheckBlock(checkpoints, 1000, wrong));
}

BOOST_AUTO_TEST_CASE(ban_fork_at_genesis_block) {
    DummyConfig config;

    // Sanity check that a checkpoint exists at the genesis block
    auto &checkpoints = config.GetChainParams().Checkpoints().mapCheckpoints;
    assert(checkpoints.find(0) != checkpoints.end());

    // Another precomputed genesis block (with differing nTime) should conflict
    // with the regnet genesis block checkpoint and not be accepted or stored
    // in memory.
    CBlockHeader header =
        CreateGenesisBlock(1296688603, 2, 0x207fffff, 1, 50 * COIN);

    // Header should not be accepted
    CValidationState state;
    CBlockHeader invalid;
    const CBlockIndex *pindex = nullptr;
    BOOST_CHECK(
        !ProcessNewBlockHeaders(config, {header}, state, &pindex, &invalid));
    BOOST_CHECK(state.IsInvalid());
    BOOST_CHECK(pindex == nullptr);
    BOOST_CHECK(invalid.GetHash() == header.GetHash());

    // Sanity check to ensure header was not saved in memory
    {
        LOCK(cs_main);
        BOOST_CHECK(LookupBlockIndex(header.GetHash()) == nullptr);
    }
}

BOOST_AUTO_TEST_SUITE_END()

struct CheckpointRegtestSetup : public TestingSetup {
    CheckpointRegtestSetup()
        : TestingSetup(CBaseChainParams::REGTEST) {}
};

BOOST_FIXTURE_TEST_SUITE(checkpoints_regtest_tests, CheckpointRegtestSetup)
class ChainParamsWithCheckpoints : public CChainParams {
public:
    ChainParamsWithCheckpoints(const CChainParams &chainParams,
                               const BlockHash &checkpoint)
        : CChainParams(chainParams) {
        checkpointData = {
            /* .mapCheckpoints = */ {
                {2, checkpoint},
            }};

    }
};

class RegtestConfigWithTestCheckpoints : public DummyConfig {
public:
    explicit RegtestConfigWithTestCheckpoints(const BlockHash &checkpoint)
        : DummyConfig(createChainParams(checkpoint)) {}

    static std::unique_ptr<CChainParams>
    createChainParams(const BlockHash &checkpoint) {
        const auto mainParams = CreateChainParams(CBaseChainParams::REGTEST);
        return std::make_unique<ChainParamsWithCheckpoints>(
            *mainParams, checkpoint);
    }
};

static CBlockHeader MakeSyntheticHeader(
    const Consensus::Params &params,
    const CBlockIndex *pindexPrev,
    uint32_t marker) {
    CBlockHeader header;

    header.nVersion = 4;
    header.hashPrevBlock = pindexPrev->GetBlockHash();
    header.hashMerkleRoot = uint256S(strprintf("%064x", marker));
    header.nTime = pindexPrev->nTime + 600 + marker;
    header.nBits = GetNextWorkRequired(pindexPrev, &header, params);
    header.nNonce = 0;

    while (!CheckProofOfWork(header.GetHash(), header.nBits, params)) {
        ++header.nNonce;
    }

    return header;
}

/**
 * This test creates 4 synthetic headers on top of the regtest genesis block: blocks mined ontop of the genesis block:
 *  G ---> A ---> AA (checkpointed)
 *   \       \
 *    \--> B  \-> AB
 * After the node has accepted only A and AA, these rejects should occur:
 *  * B should be rejected for forking prior to an accepted checkpoint
 *  * AB should be rejected for forking at an accepted checkpoint
 */

BOOST_AUTO_TEST_CASE(ban_fork_prior_to_and_at_checkpoints) {
    const auto testParams =
        RegtestConfigWithTestCheckpoints::createChainParams(BlockHash{});
    const Consensus::Params &consensus = testParams->GetConsensus();

    const CBlockIndex *pindexGenesis = nullptr;
    {
        LOCK(cs_main);
        pindexGenesis =
            LookupBlockIndex(testParams->GenesisBlock().GetHash());
    }
    BOOST_REQUIRE(pindexGenesis != nullptr);

    const CBlockHeader headerA =
        MakeSyntheticHeader(consensus, pindexGenesis, 1);

    const BlockHash hashA = headerA.GetHash();
    CBlockIndex indexA(headerA);
    indexA.phashBlock = &hashA;
    indexA.pprev = const_cast<CBlockIndex *>(pindexGenesis);
    indexA.nHeight = 1;

    const CBlockHeader headerAA =
        MakeSyntheticHeader(consensus, &indexA, 2);

    const CBlockHeader headerB =
        MakeSyntheticHeader(consensus, pindexGenesis, 3);

    const CBlockHeader headerAB =
        MakeSyntheticHeader(consensus, &indexA, 4);

    RegtestConfigWithTestCheckpoints config(headerAA.GetHash());

    CBlockHeader invalid;
    const CBlockIndex *pindex = nullptr;

    // Headers A and AA should be accepted
    {
        CValidationState state;
        BOOST_CHECK(ProcessNewBlockHeaders(config, {headerA}, state, &pindex,
                                           &invalid));
        BOOST_CHECK(state.IsValid());
        BOOST_CHECK(pindex != nullptr);
        pindex = nullptr;
        BOOST_CHECK(invalid.IsNull());
    }

    {
        CValidationState state;
        BOOST_CHECK(ProcessNewBlockHeaders(config, {headerAA}, state, &pindex,
                                           &invalid));
        BOOST_CHECK(state.IsValid());
        BOOST_CHECK(pindex != nullptr);
        pindex = nullptr;
        BOOST_CHECK(invalid.IsNull());
    }

    // Header B should be rejected
    {
        CValidationState state;
        BOOST_CHECK(!ProcessNewBlockHeaders(config, {headerB}, state, &pindex,
                                            &invalid));
        BOOST_CHECK(state.IsInvalid());
        BOOST_CHECK(state.GetRejectCode() == REJECT_CHECKPOINT);
        BOOST_CHECK(state.GetRejectReason() == "bad-fork-prior-to-checkpoint");
        BOOST_CHECK(pindex == nullptr);
        BOOST_CHECK(invalid.GetHash() == headerB.GetHash());
    }

    // Sanity check to ensure header was not saved in memory
    {
        LOCK(cs_main);
        BOOST_CHECK(LookupBlockIndex(headerB.GetHash()) == nullptr);
    }

    // Header AB should be rejected
    {
        CValidationState state;
        BOOST_CHECK(!ProcessNewBlockHeaders(config, {headerAB}, state, &pindex,
                                            &invalid));
        BOOST_CHECK(state.IsInvalid());
        BOOST_CHECK(state.GetRejectCode() == REJECT_CHECKPOINT);
        BOOST_CHECK(state.GetRejectReason() == "checkpoint mismatch");
        BOOST_CHECK(pindex == nullptr);
        BOOST_CHECK(invalid.GetHash() == headerAB.GetHash());
    }

    // Sanity check to ensure header was not saved in memory
    {
        LOCK(cs_main);
        BOOST_CHECK(LookupBlockIndex(headerAB.GetHash()) == nullptr);
    }
}

BOOST_AUTO_TEST_SUITE_END()
