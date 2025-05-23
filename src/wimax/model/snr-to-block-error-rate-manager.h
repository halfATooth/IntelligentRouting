/*
 *  Copyright (c) 2007,2008, 2009 INRIA, UDcast
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Mohamed Amine Ismail <amine.ismail@sophia.inria.fr>
 *                              <amine.ismail@udcast.com>
 */

#ifndef SNR_TO_BLOCK_ERROR_RATE_MANAGER_H
#define SNR_TO_BLOCK_ERROR_RATE_MANAGER_H

#include "snr-to-block-error-rate-record.h"

#include "ns3/ptr.h"

#include <vector>

namespace ns3
{

/**
 * @ingroup wimax
 * @brief This class handles the  SNR to BlcER traces.
 *
 * A path to a repository containing trace files should be provided.
 * If no repository is provided the traces from default-traces.h will be loaded.
 * A valid repository should contain 7 files, one for each modulation
 * and coding scheme.
 *
 * The names of the files should respect the following format:
 * \c modulation<modulation-and-conding-index>.txt, _e.g._
 * \c modulation0.txt, \c modulation1.txt, _etc._ for
 * modulation 0, modulation 1, and so on...
 *
 * The file format is ASCII with six columns as follows:
 *
 * -#  The SNR value,
 * -#  The bit error rate BER,
 * -#  The block error rate BlcERm,
 * -#  The standard deviation on block error rate,
 * -#  The lower bound confidence interval for a given modulation, and
 * -#  The upper bound confidence interval for a given modulation.
 */
class SNRToBlockErrorRateManager
{
  public:
    SNRToBlockErrorRateManager();
    ~SNRToBlockErrorRateManager();
    /**
     * @brief Set the path of the repository containing the traces
     * @param traceFilePath the path to the repository.
     */
    void SetTraceFilePath(char* traceFilePath);
    /**
     * @return the path to the repository containing the traces.
     */
    std::string GetTraceFilePath();
    /**
     * @brief returns the Block Error Rate for a given modulation and SNR value
     * @param SNR the SNR value
     * @param modulation one of the seven MCS
     * @return the Block Error Rate
     */
    double GetBlockErrorRate(double SNR, uint8_t modulation);
    SNRToBlockErrorRateRecord*
    /**
     * @brief returns a record of type SNRToBlockErrorRateRecord corresponding to a given modulation
     * and SNR value
     * @param SNR the SNR value
     * @param modulation one of the seven MCS
     * @return the Block Error Rate
     */
    GetSNRToBlockErrorRateRecord(double SNR, uint8_t modulation);
    /**
     * @brief Loads the traces form the repository specified in the constructor or set by
     * SetTraceFilePath function. If no repository is provided, default traces will be loaded from
     * default-traces.h file
     */

    void LoadTraces();
    /**
     * @brief Loads the default traces from default-traces.h file
     */
    void LoadDefaultTraces();
    /**
     * @brief Reloads the trace
     */
    void ReLoadTraces();
    /**
     * @brief If activate loss is called with false, all the returned BlcER will be 0 (no losses)
     * @param loss true to activates losses
     */
    void ActivateLoss(bool loss);

  private:
    /// Clear records function
    void ClearRecords();
    bool m_activateLoss;         ///< activate loss
    std::string m_traceFilePath; ///< trace file path

    std::vector<SNRToBlockErrorRateRecord*>* m_recordModulation[7]; ///< record modulation
};
} // namespace ns3

#endif /* SNR_TO_BLOCK_ERROR_RATE_MANAGER_H */
