// Copyright 2021 DeepMind Technologies Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string>
#include <sstream>

#include "open_spiel/abseil-cpp/absl/flags/flag.h"
#include "open_spiel/abseil-cpp/absl/flags/parse.h"
#include "open_spiel/algorithms/cfr.h"
#include "open_spiel/algorithms/tabular_exploitability.h"
#include "open_spiel/spiel.h"
#include "open_spiel/spiel_utils.h"

ABSL_FLAG(int, num_iters, 10000, "How many iters to run for.");
ABSL_FLAG(int, report_every, 200, "How often to report exploitability.");
ABSL_FLAG(int, num_players, 3, "The number of players in the game.");
ABSL_FLAG(int, ante_amount, 1, "How many chips each player initially contributes tot the pot.");
ABSL_FLAG(bool, alternating, false, "Use CFR with alternating updates");

void run(std::shared_ptr<const open_spiel::Game> game, int ante) {
  std::cerr << "ANTE: " << ante << std::endl;
  open_spiel::algorithms::CFRSolverBase solver(*game,
      /*alternating_updates*/absl::GetFlag(FLAGS_alternating),
      /*linear_averaging*/false, /*regret_matching_plus*/false);
  for (int i = 0; i < absl::GetFlag(FLAGS_num_iters); ++i) {
    solver.EvaluateAndUpdatePolicy();
    if (i % absl::GetFlag(FLAGS_report_every) == 0 ||
        i == absl::GetFlag(FLAGS_num_iters) - 1) {
      double nash_conv = open_spiel::algorithms::NashConv(
          *game, *solver.AveragePolicy());
      std::cerr << "Iteration " << i << " NashConv=" << nash_conv
                << std::endl;
    }
  }
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  std::stringstream ss;
  int ante = absl::GetFlag(FLAGS_ante_amount);
  ss << "leduc_poker(players=3,ante_amount=" << ante << ")";
  std::shared_ptr<const open_spiel::Game> game = open_spiel::LoadGame(ss.str());
  run(game, ante);
}
