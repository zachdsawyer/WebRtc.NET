/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef WEBRTC_MODULES_CONGESTION_CONTROLLER_TRENDLINE_ESTIMATOR_H_
#define WEBRTC_MODULES_CONGESTION_CONTROLLER_TRENDLINE_ESTIMATOR_H_

#include <list>
#include <utility>

#include "webrtc/base/constructormagic.h"
#include "webrtc/common_types.h"

namespace webrtc {

class TrendlineEstimator {
 public:
  // |window_size| is the number of points required to compute a trend line.
  // |smoothing_coef| controls how much we smooth out the delay before fitting
  // the trend line. |threshold_gain| is used to scale the trendline slope for
  // comparison to the old threshold. Once the old estimator has been removed
  // (or the thresholds been merged into the estimators), we can just set the
  // threshold instead of setting a gain.
  TrendlineEstimator(size_t window_size,
                     double smoothing_coef,
                     double threshold_gain);
  ~TrendlineEstimator();

  // Update the estimator with a new sample. The deltas should represent deltas
  // between timestamp groups as defined by the InterArrival class.
  void Update(double recv_delta_ms, double send_delta_ms, double now_ms);

  // Returns the estimated trend k multiplied by some gain.
  // 0 < k < 1   ->  the delay increases, queues are filling up
  //   k == 0    ->  the delay does not change
  //   k < 0     ->  the delay decreases, queues are being emptied
  double trendline_slope() const { return trendline_ * threshold_gain_; }

  // Returns the number of deltas which the current estimator state is based on.
  unsigned int num_of_deltas() const { return num_of_deltas_; }

 private:
  // Parameters.
  const size_t window_size_;
  const double smoothing_coef_;
  const double threshold_gain_;
  // Used by the existing threshold.
  unsigned int num_of_deltas_;
  // Exponential backoff filtering.
  double accumulated_delay_;
  double smoothed_delay_;
  // Linear least squares regression.
  std::list<std::pair<double, double>> delay_hist_;
  double trendline_;

  RTC_DISALLOW_COPY_AND_ASSIGN(TrendlineEstimator);
};
}  // namespace webrtc

#endif  // WEBRTC_MODULES_CONGESTION_CONTROLLER_TRENDLINE_ESTIMATOR_H_