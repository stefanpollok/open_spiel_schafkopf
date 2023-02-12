// Copyright 2019 DeepMind Technologies Limited
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

#ifndef OPEN_SPIEL_GAMES_SCHAFKOPF_H_
#define OPEN_SPIEL_GAMES_SCHAFKOPF_H_

#include <string>

#include "open_spiel/spiel.h"

// See https://en.wikipedia.org/wiki/Schafkopf
// Schafkopf is a popular German trick-taking card game of the Ace-Ten family
// for four players that evolved, towards the end of the 19th century, from
// German Schafkopf. It is still very popular in Bavaria, where it is their
// national card game played by around two million people, but it also played
// elsewhere in Germany and in Austria. It is an official cultural asset and
// important part of the Old Bavarian and Franconian way of life. Schafkopf
// is a mentally demanding pastime that is considered "the supreme discipline
// of Bavarian card games" and "the mother of all trump games." 
//
// CURRENT LIMITATIONS
//
// GAME CHARACTERISTICS
// Sauspiel with Rufsau
// Hochzeit
// (Farb-)Wenz
// (Farb-)Geier
// Farb-Solo
// Ramsch
// Stock
//
// ACTION SPACE

namespace open_spiel {
namespace schafkopf {

inline constexpr int kNumRanks = 8;
// Suits: Eichel, Gras, Herz, Schellen
inline constexpr int kNumSuits = 4;
inline constexpr int kNumCards = kNumRanks * kNumSuits;
inline constexpr int kNumPlayers = 4;
inline constexpr int kNumPhases = 5;
inline constexpr int kNumGameTypes = 18;
inline constexpr int kNumTricks = kNumCards / kNumPlayers;
inline constexpr int kBiddingActionBase = kNumCards;  // First bidding action after card action id.
inline constexpr int kRufenActionBase = kNumCards + kNumGameTypes;
inline constexpr int kSteigernActionBase = kNumCards + kNumGameTypes + kNumSuits;
inline constexpr int kNumSteigernGameBase = 10;
inline constexpr int kNumBiddingActions = kNumGameTypes;
inline constexpr int kNumActions = kNumCards + kNumBiddingActions + kNumSuits + 7;
inline constexpr char kEmptyCardSymbol[] = "🂠";

inline constexpr int kObservationTensorSize =
    kNumPlayers                    // Player position
    + kNumPhases                      // Phase
    + kNumCards                    // Players cards
    + kNumPlayers * kNumGameTypes  // All players' bids
    + kNumPlayers                  // Who's playing
    + kNumGameTypes                // Game type
    + kNumPlayers                  // Who started the current trick
    + kNumPlayers * kNumCards      // Cards played to the current trick
    + kNumPlayers                  // Who started the previous trick
    + kNumPlayers * kNumCards;     // Cards played to the previous trick

enum SchafkopfGameType {
  kUnknownGame = 0,
  kPass = 0,
  kSauspiel=1,
  kHochzeit=2,
  kGeier_Schellen=3,
  kGeier_Herz=4,
  kGeier_Gras=5,
  kGeier_Eichel=6,
  kWenz_Schellen=7,
  kWenz_Herz=8,
  kWenz_Gras=9,
  kWenz_Eichel=10,
  kGeier=11,
  kWenz=12,
  kSolo_Schellen=13,
  kSolo_Herz=14,
  kSolo_Gras=15,
  kSolo_Eichel=16,
  kRamsch = 17,
  kStock = 18,
};
enum Suit {
  kSchellen = 0,
  kHerz = 1,
  kGras = 2,
  kEichel = 3
};
enum Rank {
  kSeven = 0,
  kEight = 1,
  kNine = 2,
  kTen = 3,
  kUnter = 4,
  kOber = 5,
  kKing = 6,
  kAce = 7
};
enum CardLocation{
  kDeck = 0,
  kHand0 = 1,
  kHand1 = 2,
  kHand2 = 3,
  kHand3 = 4,
  kTrick = 5,
  kHochzeiter = 6
};
enum Phase {
  kDeal = 0,
  kBidding = 1,
  kRufen = 2,
  kHeiraten = 3,
  kSteigern = 4,
  kPlay = 5,
  kGameOver = 6
};

// This is the information about one trick, i.e. up to four cards where each
// card was played by one player.
// Stich
class Trick {
 public:
  Trick() : Trick{-1} {}
  Trick(Player leader) { leader_ = leader; }
  int FirstCard() const;
  Player Leader() const { return leader_; }
  // How many cards have been played in the trick. Between 0 and 4.
  int CardsPlayed() const { return cards_.size(); }
  // Returns a vector of the cards played in this trick. These are ordered by
  // the order of play, i.e. the first card is not necessarily played by player
  // 1 but by the player who played first in this trick.
  std::vector<int> GetCards() const { return cards_; }
  // Adds `card` to the trick as played by player with id `player`.
  void PlayCard(int card);
  // Returns the player id of the player who was at position `position` in this
  // trick. Position is 0 based here, i.e. PlayerAtPosition(0) returns the
  // player who played the first card in this trick. This method fails if no
  // cards have been played yet.
  int PlayerAtPosition(int position) const;
  // Returns the sum of the values of the cards in the trick.
  int Points() const;
  std::string ToString() const;

 private:
  std::vector<int> cards_{};
  Player leader_;
  Suit led_suit_;
};

class SchafkopfState : public State {
 public:
  SchafkopfState(std::shared_ptr<const Game> game);
  SchafkopfState(const SchafkopfState&) = default;

  Player CurrentPlayer() const override {
    return IsTerminal() ? kTerminalPlayerId : current_player_;
  }

  std::string ActionToString(Player player, Action action_id) const override;
  bool IsTerminal() const override { return phase_ == kGameOver; }
  std::vector<double> Returns() const override { return returns_; }
  std::unique_ptr<State> Clone() const override {
    return std::unique_ptr<State>(new SchafkopfState(*this));
  }
  std::string ToString() const override;
  std::vector<Action> LegalActions() const override;
  std::vector<std::pair<Action, double>> ChanceOutcomes() const override;

  std::string ObservationString(Player player) const override;
  void ObservationTensor(Player player,
                         absl::Span<float> values) const override;

 protected:
  void DoApplyAction(Action action) override;

 private:
  std::vector<Action> DealLegalActions() const;
  std::vector<Action> BiddingLegalActions() const;
  std::vector<Action> SteigernLegalActions() const;
  std::vector<Action> RufenLegalActions() const;
  std::vector<Action> PlayLegalActions() const;
  void ApplyDealAction(int card);
  void ApplyBiddingAction(int game_type);
  void ApplySteigernAction(int game_type);
  void ApplyRufenAction(int rufsau);
  void ApplyPlayAction(int card);

  void EndBidding(Player winner, SchafkopfGameType game_type);
  int NextPlayer() { return (current_player_ + 1) % kNumPlayers; }
  bool IsTrump(int card) const;
  int CardOrder(int card, int first_card) const;
  int TrumpOrder(int card) const;
  int NullOrder(Rank rank) const;
  int WinsTrick() const;
  void ScoreUp();
  int CurrentTrickIndex() const {
    return std::min(kNumTricks - 1, num_cards_played_ / kNumPlayers);
  }
  Trick& CurrentTrick() { return tricks_[CurrentTrickIndex()]; }
  const Trick& CurrentTrick() const { return tricks_[CurrentTrickIndex()]; }
  const Trick& PreviousTrick() const {
    return tricks_[std::max(0, num_cards_played_ / kNumPlayers - 1)];
  }
  std::string CardLocationsToString() const;

  SchafkopfGameType game_type_ = kUnknownGame;
  Phase phase_ = kDeal;
  // CardLocation for each card.
  std::array<CardLocation, kNumCards> card_locations_;
  std::array<int, kNumPlayers> player_bids_;
  int highest_game_ = 0;
  int highest_player_ = 0;
  int rufsau_;

  // Play related.
  Player spieler_ = kChancePlayerId;
  Player partner_ = -1;
  Player current_player_ = kChancePlayerId;  // The player next to make a move.
  Player last_trick_winner_ = kChancePlayerId;
  int num_cards_played_ = 0;
  std::array<Trick, kNumTricks> tricks_{};  // Tricks played so far.
  int points_spieler_ = 0;
  int points_nicht_spieler_ = 0;
  int game_cost_ = 0;
  std::vector<double> returns_ = std::vector<double>(kNumPlayers);
};

class SchafkopfGame : public Game {
 public:
  explicit SchafkopfGame(const GameParameters& params);
  int NumDistinctActions() const override { return kNumActions; }
  std::unique_ptr<State> NewInitialState() const override;
  int NumPlayers() const override { return kNumPlayers; }
  double MinUtility() const override { return -1.0; }
  double MaxUtility() const override { return  1.0; }
  double UtilitySum() const override { return 0; }
  int MaxGameLength() const override { return kNumCards + kNumPlayers; }
  // TODO: verify whether this bound is tight and/or tighten it.
  int MaxChanceNodesInHistory() const override { return MaxGameLength(); }
  int MaxChanceOutcomes() const override { return kNumCards; }
  std::vector<int> ObservationTensorShape() const override {
    return {kObservationTensorSize};
  }
};

}  // namespace schafkopf
}  // namespace open_spiel

#endif  // OPEN_SPIEL_GAMES_SCHAFKOPF_H_
