# Nouveau-Vision Unit Node (NVUN)

Nouveau-Vision Unit Node (NVUN) is the full-node implementation for the Nouveau-Vision Unit (NVU) peer-to-peer blockchain network.

NVU is an independent SHA-256 proof-of-work digital currency built around direct participation, independently operated nodes, and accessible mining.

Project website: https://nvu.us

## Nouveau-Vision Unit

Nouveau-Vision Unit is an independent blockchain with its own genesis block, network identifiers, peer-to-peer network, address prefix, proof-of-work schedule, and chain history.

The network is designed around a principle expressed in its genesis message:

> Nouveau-Vision Unit 27/Oct/2026 Small miners deserve a meaningful place in SHA-256

## Network Parameters

| Parameter | NVU Mainnet |
| --- | --- |
| Project | Nouveau-Vision |
| Currency | Nouveau-Vision Unit |
| Ticker | NVU |
| Full node | NVUN |
| Proof of work | SHA-256d |
| Target block interval | 10 minutes |
| Initial block subsidy | 50 NVU |
| Subsidy halving interval | 210,000 blocks |
| Coinbase maturity | 100 blocks |
| CashAddr prefix | nvu: |
| P2P port | 9333 |
| RPC port | 9332 |

NVU uses ASERT difficulty adjustment with a two-day half-life.

The launch difficulty was designed around approximately 5 TH/s of SHA-256 network hashrate so that smaller and independently operated miners can remain meaningful network participants.

## Genesis Block

NVU has a unique genesis block.

### Genesis Message

Nouveau-Vision Unit 27/Oct/2026 Small miners deserve a meaningful place in SHA-256

### Genesis Hash

000000003ab967d9379a450afdb1f7e849434ff7bc48d99a4efef91be531ae55

### Merkle Root

5d2dfd9f027626e2ce0e056d35d3e82e49d9b116c9903c761c813f931b874d73

### Genesis Parameters

Timestamp: 1793122760
Nonce: 3464907508
Bits: 0x1d00ffff
Version: 1
Reward: 50 NVU

## Proof of Work and Difficulty

NVU uses SHA-256d proof of work with a target block interval of approximately 600 seconds.

Difficulty adjustment is performed using ASERT with a two-day half-life.

NVU mainnet does not permit automatic minimum-difficulty blocks, and proof-of-work retargeting remains enabled.

The NVU ASERT anchor begins at block 1 with a launch target selected around the network's approximately 5 TH/s launch design.

## Monetary Policy

NVU begins with a block subsidy of 50 NVU.

The block subsidy halves every 210,000 blocks.

Coinbase outputs require 100 blocks of maturity before they can be spent.

The current consensus monetary-range constant is 21,000,000 NVU.

## Addresses

NVU's canonical CashAddr prefix is:

nvu:

NVU applications and wallets should preferentially display the nvu: address representation.

Legacy key and address encodings inherited from the upstream codebase remain under review as part of the NVUN 1.0 specification process.

Existing wallet compatibility will be considered before any change to those formats.

## Peer-to-Peer Network

NVU operates an independent peer-to-peer network.

Default network ports are:

P2P: 9333
RPC: 9332

NVU uses its own network and disk magic values to distinguish NVU traffic and blockchain data from other networks.

The NVU peer network is independent from Bitcoin and Bitcoin Cash, allowing NVUN to coexist with Bitcoin and Bitcoin Cash nodes on the same host when separately configured.

## Repository Branches

### nvu-main

Stable source corresponding to the current NVU mainnet implementation.

### nvun-v1-dev

Active development branch for NVUN 1.0.

NVUN 1.0 development work is built and tested separately before deployment to the live NVU network.

## Development Checkpoints

The repository contains tags identifying important points in NVU development.

### nvu-bchn-baseline

The upstream Bitcoin Cash Node source baseline from which NVU development began.

### nvu-pre-v1-consensus-checkpoint

Known-good NVU mainnet source immediately before NVUN 1.0 consensus, release-identity, and source-cleanup work began.

This checkpoint corresponds to source commit:

50a65dbb267dee22e8f6c4c316787f8f38b44a6d

The associated live-chain checkpoint was:

Height: 1652

Best block:
000000000000033015aed4fac014266c997bae16dfb19ec477a455ffa28cb9cf

## Building NVUN

NVUN uses CMake.

A headless Linux build can be configured with:

cmake -S . -B build -DBUILD_BITCOIN_QT=OFF

The primary NVU binaries are:

nvund
nvu-cli
nvu-tx

Build configuration and dependency documentation inherited from the upstream codebase remains available under the doc/ directory while NVUN-specific documentation continues to be developed.

## NVUN 1.0 Development

NVUN 1.0 is currently under development.

The v1 development process includes an audit of:

- consensus parameters
- inherited future-upgrade behavior
- software release identity
- software-expiration behavior
- peer discovery and bootstrap infrastructure
- packaging
- documentation
- user-facing project identity
- chain-specific configuration

Consensus-related changes are treated separately from cosmetic, documentation, packaging, and branding changes.

NVUN 1.0 development is being performed from the nvun-v1-dev branch while nvu-main remains the stable reference for the currently operating network.

## Future Network Upgrades

NVU does not automatically adopt future consensus upgrades solely because they are scheduled by an upstream project.

Future NVU consensus upgrades require deliberate evaluation for the Nouveau-Vision Unit network, including implementation review, testing, documentation, release planning, and an explicit NVU activation decision.

This approach allows NVU to benefit from relevant upstream development while maintaining an independently defined network specification.

## Project Status

NVU mainnet is operational.

NVUN nodes have successfully participated in the independent NVU peer-to-peer network and validated the NVU blockchain from its unique genesis block.

NVUN 1.0 development is focused on formalizing the network's existing identity and behavior before declaring the v1 software and network specification complete.

## Source Lineage and Attribution

NVUN is derived from Bitcoin Cash Node (BCHN).

Bitcoin Cash Node in turn contains code originating from Bitcoin Core, Bitcoin ABC, and numerous other contributors and projects.

NVUN preserves upstream Git history, authorship, copyright notices, and license information.

The presence of upstream project names within source files, historical documentation, tests, libraries, protocol terminology, and copyright notices does not imply that those upstream projects operate, sponsor, or endorse the Nouveau-Vision Unit network.

### Upstream Bitcoin Cash Node

https://gitlab.com/bitcoin-cash-node/bitcoin-cash-node

### NVUN Source Repository

https://github.com/thenouveauvision/nvun

## License

NVUN is distributed under the MIT software license inherited from the upstream codebase.

See COPYING for the applicable license text and copyright information.

Existing copyright and attribution notices belonging to upstream authors and projects are intentionally preserved.

## Nouveau-Vision

Project: Nouveau-Vision
Currency: Nouveau-Vision Unit
Ticker: NVU
Node: NVUN
Website: https://nvu.us
Source: https://github.com/thenouveauvision/nvun
