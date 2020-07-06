// Copyright (c) 2020 The Zcash developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#include <consensus/funding.h>

namespace Consensus
{
/**
 * General information about each funding stream.
 * Ordered by Consensus::FundingStreamIndex.
 */
const struct FSInfo FundingStreamInfo[Consensus::MAX_FUNDING_STREAMS] = {
    {
        .recipient = "Electric Coin Company",
        .specification = "https://zips.z.cash/zip-0214",
        .valueNumerator = 7,
        .valueDenominator = 100,
    },
    {
        .recipient = "Zcash Foundation",
        .specification = "https://zips.z.cash/zip-0214",
        .valueNumerator = 5,
        .valueDenominator = 100,
    },
    {
        .recipient = "Major Grants",
        .specification = "https://zips.z.cash/zip-0214",
        .valueNumerator = 8,
        .valueDenominator = 100,
    }
};

CAmount FSInfo::Value(CAmount blockSubsidy) const
{
    // Integer division is floor division for nonnegative integers in C++
    return CAmount((blockSubsidy * valueNumerator) / valueDenominator);
}

std::set<FundingStreamElement> GetActiveFundingStreamElements(
    int nHeight,
    CAmount blockSubsidy,
    const Consensus::Params& params)
{
    std::set<std::pair<FundingStreamAddress, CAmount>> requiredElements;
    for (int idx = Consensus::FIRST_FUNDING_STREAM; idx < Consensus::MAX_FUNDING_STREAMS; idx++) {
        auto fs = params.vFundingStreams[idx];
        // Funding period is [startHeight, endHeight)
        if (fs && nHeight >= fs.get().GetStartHeight() && nHeight < fs.get().GetEndHeight()) {
            requiredElements.insert(std::make_pair(
                fs.get().RecipientAddress(params, nHeight),
                FundingStreamInfo[idx].Value(blockSubsidy)));
        }
    }
    return requiredElements;
};

int GetMaxFundingStreamHeight(const Consensus::Params& params) {
    int result = 0;
    for (int idx = Consensus::FIRST_FUNDING_STREAM; idx < Consensus::MAX_FUNDING_STREAMS; idx++) {
        auto fs = params.vFundingStreams[idx];
        if (fs && result < fs.get().GetEndHeight() - 1) {
            result = fs.get().GetEndHeight() - 1;
        }
    }

    return result;
}

} // namespace Consensus
