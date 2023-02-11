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

#include "open_spiel/games/schafkopf.h"

#include "open_spiel/abseil-cpp/absl/strings/str_format.h"
#include "open_spiel/abseil-cpp/absl/strings/string_view.h"
#include "open_spiel/game_parameters.h"
#include "open_spiel/spiel.h"
#include "open_spiel/spiel_utils.h"


namespace open_spiel {
namespace schafkopf {
namespace {

const GameType kGameType{/*short_name=*/"Sk",
                         /*long_name=*/"Schafkopf",
                         GameType::Dynamics::kSequential,
                         GameType::ChanceMode::kExplicitStochastic,
                         GameType::Information::kImperfectInformation,
                         GameType::Utility::kZeroSum,
                         GameType::RewardModel::kTerminal,
                         /*max_num_players=*/4,
                         /*min_num_players=*/4,
                         /*provides_information_state_string=*/false,
                         /*provides_information_state_tensor=*/false,
                         /*provides_observation_string=*/true,
                         /*provides_observation_tensor=*/true,
                         /*parameter_specification=*/{}  // no parameters
};

std::shared_ptr<const Game> Factory(const GameParameters& params) {
  return std::shared_ptr<const Game>(new SchafkopfGame(params));
}

REGISTER_SPIEL_GAME(kGameType, Factory);


}  // namespace

Suit CardSuit(int card) {
  return static_cast<Suit>(card / 8);
}

Rank CardRank(int card) {
  return static_cast<Rank>(card % 8);
}
// Schellen : Diamonds
// Blatt / Gras : Spades
// Herz : Hearts
// Eichel : Clubs / Kreuz
// Unter : Jack
// Ober : Queen
const std::vector<std::string> kCardSymbols = {
  "🃇", "🃈", "🃉", "🃊", "🃋", "🃍", "🃎", "🃁",
  "🂷", "🂸", "🂹", "🂺", "🂻", "🂽", "🂾", "🂱",
  "🂧", "🂨", "🂩", "🂪", "🂫", "🂭", "🂮", "🂡",
  "🃗", "🃘", "🃙", "🃚", "🃛", "🃝", "🃞", "🃑"
};


std::string ToCardSymbol(int card) {
  if (card >= 0) {
    return kCardSymbols.at(card);
  } else {
    return kEmptyCardSymbol;
  }
}

std::string SuitToString(Suit suit) {
  switch (suit) {
    case kEichel:
      return "E";
    case kGras:
      return "G";
    case kHerz:
      return "H";
    case kSchellen:
      return "S";
    default:
      return "error";
  }
}

std::string RankToString(Rank rank) {
  switch (rank) {
    case kSeven:
      return "7";
    case kEight:
      return "8";
    case kNine:
      return "9";
    case kTen:
      return "10";
    case kUnter:
      return "U";    
    case kOber:
      return "O";
    case kKing:
      return "K";
    case kAce:
      return "A";
    default:
      return "error";
  }
}

std::string PhaseToString(Phase phase) {
  switch (phase) {
    case kDeal:
      return "dealing";
    case kBidding:
      return "bidding";
    case kSteigern:
      return "steigern";
    case kRufsau:
      return "calling Rufsau";
    case kHeiraten:
      return "Hochzeit";
    case kPlay:
      return "playing";
    case kGameOver:
      return "game over";
    default:
      return "error";
  }
}

int CardValue(int card) {
  switch (CardRank(card)) {
    case kUnter:
      return 2;
    case kOber:
      return 3;
    case kKing:
      return 4;
    case kTen:
      return 10;
    case kAce:
      return 11;
    default:
      return 0;  // Seven, eight and nine.
  }
}

std::string CardToString(int card) {
  return SuitToString(CardSuit(card)) + RankToString(CardRank(card));
}

std::string CardsToString(const std::vector<int>& cards) {
  std::string result = "";
  for (auto& card : cards) {
    absl::StrAppendFormat(&result, "%s ", ToCardSymbol(card));
  }
  return result;
}


std::string SchafkopfGameTypeToString(SchafkopfGameType trump_game) {
  switch (trump_game) {
    case kUnknownGame:
      return "unknown/pass";
    case kSauspiel:
      return "Saupsiel";
    case kHochzeit:
      return "Hochzeit";
    case kWenz_Eichel:
      return "Eichel-Wenz";
    case kWenz_Gras:
      return "Gras-Wenz";
    case kWenz_Herz:
      return "Herz-Wenz";
    case kWenz_Schellen:
      return "Schellen-Wenz";
    case kWenz:
      return "Wenz";
    case kGeier_Eichel:
      return "Eichel-Geier";
    case kGeier_Gras:
      return "Gras-Geier";
    case kGeier_Herz:
      return "Herz-Geier";
    case kGeier_Schellen:
      return "Schellen-Geier";
    case kGeier:
      return "Geier";
    case kSolo_Eichel:
      return "Eichel-Solo";
    case kSolo_Gras:
      return "Gras-Solo";
    case kSolo_Herz:
      return "Herz-Solo";
    case kSolo_Schellen:
      return "Schellen-Solo";
    case kRamsch:
      return "Ramsch";
    case kStock:
      return "Stock";
    default:
      return "error";
  }
}

CardLocation PlayerToLocation(int player) {
  switch (player) {
    case 0:
      return kHand0;
    case 1:
      return kHand1;
    case 2:
      return kHand2;
    case 3:
      return kHand3;
    case 4:
      return kHochzeiter;
    default:
      return kDeck;
  }
}

// *********************************** Stich / Trick ***********************************

int Trick::FirstCard() const {
  if (cards_.empty()) {
    return -1;
  } else {
    return cards_[0];
  }
}

void Trick::PlayCard(int card) {
  SPIEL_CHECK_LE(cards_.size(), kNumPlayers);
  cards_.push_back(card);
}

int Trick::PlayerAtPosition(int position) const {
  SPIEL_CHECK_GE(position, 0);
  SPIEL_CHECK_LE(position, 3);
  return (leader_ + position) % kNumPlayers;
}

int Trick::Points() const {
  int sum = 0;
  for (auto& card : cards_) {
    sum += CardValue(card);
  }
  return sum;
}

std::string Trick::ToString() const {
  std::string result = absl::StrFormat("Leader: %d, ", leader_);
  for (auto& card : cards_) {
    if (card >= 0 && card < kNumCards)
      absl::StrAppendFormat(&result, "%s ", ToCardSymbol(card));
    else
      absl::StrAppendFormat(&result, "%s ", kEmptyCardSymbol);
  }
  return result;
}

// ********************************* SchafkopfState *********************************

SchafkopfState::SchafkopfState(std::shared_ptr<const Game> game)
      : State(game) {
  card_locations_.fill(kDeck);
  player_bids_.fill(kPass);
}

std::string SchafkopfState::ActionToString(Player player, Action action_id) const {
  if (action_id < kBiddingActionBase) {
    return CardToString(action_id);
  } else {
    return SchafkopfGameTypeToString(
        static_cast<SchafkopfGameType>(action_id - kBiddingActionBase));
  }
}

std::string SchafkopfState::ToString() const {
  std::string result = "";
  absl::StrAppendFormat(&result, "Phase: %s \n", PhaseToString(phase_));
  absl::StrAppendFormat(&result, "Current Player: %d", current_player_);
  absl::StrAppendFormat(&result, "\n%s\n", CardLocationsToString());
  if (phase_ == kPlay || phase_ == kGameOver) {
    absl::StrAppendFormat(&result, "Last trick won by player %d\n",
                          last_trick_winner_);
    absl::StrAppendFormat(&result, "Spieler: %d\n", spieler_);
    absl::StrAppendFormat(&result, "Points (Spieler / Nicht-Spieler): (%d / %d)\n",
                          points_spieler_, points_nicht_spieler_);
    absl::StrAppendFormat(&result, "Current Trick: %s\n",
                          CurrentTrick().ToString());
    if (CurrentTrickIndex() > 0) {
      absl::StrAppendFormat(&result, "Last Trick: %s\n",
                            PreviousTrick().ToString());
    }
  }
  absl::StrAppendFormat(&result, "Game Type: %s\n",
                SchafkopfGameTypeToString(game_type_));
  return result;
}

bool SchafkopfState::IsTrump(int card) const {
  // Herz is usually the trump suit, unless otherwise declared.
  // In Wenz (only Unter) and Geier (only Ober), there is no trump suit.
  // Ober are the highest trump, followed by the four Unter.
  if ((CardRank(card) == kOber || CardRank(card) == kUnter) &&
      (game_type_ == kSauspiel || game_type_ == kHochzeit || game_type_ == kRamsch ||
       game_type_ == kSolo_Eichel || game_type_ == kSolo_Gras ||
       game_type_ == kSolo_Herz || game_type_ == kSolo_Schellen)) return true;

  if (CardRank(card) == kUnter && (game_type_ == kWenz || game_type_ == kWenz_Eichel ||
      game_type_ == kWenz_Gras || game_type_ == kWenz_Herz || game_type_ == kWenz_Schellen)) return true;

  if (CardRank(card) == kOber && (game_type_ == kGeier || game_type_ == kGeier_Eichel ||
      game_type_ == kGeier_Gras || game_type_ == kGeier_Herz || game_type_ == kGeier_Schellen)) return true;

  switch (game_type_) {
    case kSauspiel:
      return CardSuit(card) == kHerz;
    case kHochzeit:
      return CardSuit(card) == kHerz;
    case kRamsch:
      return CardSuit(card) == kHerz;
    case kWenz_Eichel:
      return CardSuit(card) == kEichel;
    case kWenz_Gras:
      return CardSuit(card) == kGras;
    case kWenz_Herz:
      return CardSuit(card) == kHerz;
    case kWenz_Schellen:
      return CardSuit(card) == kSchellen;
    case kGeier_Eichel:
      return CardSuit(card) == kEichel;
    case kGeier_Gras:
      return CardSuit(card) == kGras;
    case kGeier_Herz:
      return CardSuit(card) == kHerz;
    case kGeier_Schellen:
      return CardSuit(card) == kSchellen;
    case kSolo_Eichel:
      return CardSuit(card) == kEichel;
    case kSolo_Gras:
      return CardSuit(card) == kGras;
    case kSolo_Herz:
      return CardSuit(card) == kHerz;
    case kSolo_Schellen:
      return CardSuit(card) == kSchellen;
    default:
      return false;
  }
}

int SchafkopfState::CardOrder(int card, int first_card) const {
  if (IsTrump(card)) {
    return kNumRanks + TrumpOrder(card);
  } else if (CardSuit(card) == CardSuit(first_card)) {  // Following suit.
    return static_cast<int>(CardRank(card));
  } else {
    return -1;
  }
}

int SchafkopfState::TrumpOrder(int card) const {
  if (!IsTrump(card)) {
    return -1;
  } else if (CardRank(card) == kOber) {
    return static_cast<int>(CardSuit(card)) + kNumRanks + kNumSuits;
  } else if (CardRank(card) == kUnter) {
    return static_cast<int>(CardSuit(card)) + kNumRanks;
  } else {
    return static_cast<int>(CardRank(card));
  }
}

int SchafkopfState::WinsTrick() const {
  std::vector<int> cards = PreviousTrick().GetCards();
  if (cards.empty()) return -1;
  int winning_position = 0;
  for (int i = 1; i < cards.size(); i++) {
    if (CardOrder(cards[i], cards[0]) >
        CardOrder(cards[winning_position], cards[0])) {
      winning_position = i;
    }
  }
  return PreviousTrick().PlayerAtPosition(winning_position);
}

void SchafkopfState::DoApplyAction(Action action) {
  switch (phase_) {
    case kDeal:
      return ApplyDealAction(action);
    case kBidding:
      return ApplyBiddingAction(action - kBiddingActionBase);
    case kSteigern:
      return ApplySteigernAction(action);
    case kRufsau:
      return ApplyRufsauAction(action);
    case kPlay:
      return ApplyPlayAction(action);
    case kGameOver:
      SpielFatalError("Cannot act in terminal states");
  }
}

// TODO: Possibly two deal action with "Klopfer" in between
void SchafkopfState::ApplyDealAction(int card) {
  SPIEL_CHECK_EQ(card_locations_[card], kDeck);
  int deal_round = history_.size();
  // Cards 0-4, 17-20 to player 1.
  // Cards 5-8, 21-24 to player 2.
  // Cards 9-12, 25-28 to player 3.
  // Cards 13-16, 29-32 to player 4.
  if ((deal_round >= 0 && deal_round <= 3) ||
      (deal_round >= 16 && deal_round <= 19)) {
    card_locations_[card] = kHand0;
  } else if ((deal_round >= 4 && deal_round <= 7) ||
      (deal_round >= 20 && deal_round <= 23)) {
    card_locations_[card] = kHand1;
  } else if ((deal_round >= 8 && deal_round <= 11) ||
      (deal_round >= 24 && deal_round <= 27)) {
    card_locations_[card] = kHand2;
  } else if ((deal_round >= 12 && deal_round <= 15) ||
      (deal_round >= 28 && deal_round <= 31)) {
    card_locations_[card] = kHand3;
  }
  if (deal_round == kNumCards - 1) {
    current_player_ = 0;
    phase_ = kBidding;
  }
}

// TODO: Add Hochzeit
void SchafkopfState::ApplyBiddingAction(int game_type) {
  // if (!steigern_) player_bids_[current_player_] = game_type;
  // highest_game_ = *std::max_element(player_bids_.begin(), player_bids_.end());
  // highest_player_ = std::distance(player_bids_.begin(), std::max_element(player_bids_.begin(), player_bids_.end()));

  // Check for new highest game
  if (game_type > highest_game_) {
    highest_game_ = game_type;
    highest_player_ = current_player_;
  }
  // All four players have made a choice      
  if (current_player_ == 3) {
    // Check for Ramsch
    if (highest_game_ == kPass) {
      EndBidding(-1, SchafkopfGameType(kRamsch));
      return;
    // Check for Solo, but let others player a chance to react
    } else if (highest_game_ > kSauspiel) {
      for (int player = 0; player < kNumPlayers; player++) {
        if (player_bids_[player] > kSauspiel && player != highest_player_) {
          current_player_ = player;
          phase_ = kSteigern;
          return;
        }
      }
    }
    EndBidding(highest_player_, SchafkopfGameType(highest_game_));
  }
  current_player_ = NextPlayer();
}

void SchafkopfState::ApplySteigernAction(int game_type) {
  if (game_type_ <= highest_game_) {
    player_bids_[current_player_] = kPass;
    for (int player = 0; player < kNumPlayers; player++) {
      if (player_bids_[player] > kSauspiel) {
        current_player_ = player;
        return;
      }
    }
  // Increase player's game choice and let next player react
  } else {
    player_bids_[current_player_] = game_type;
    highest_game_ = game_type;
    highest_player_ = current_player_;
    for (int player = 0; player < kNumPlayers; player++) {
      if (player_bids_[player] > kSauspiel && player != current_player_) {
        current_player_ = player;
        return;
      }
    }
  }
  EndBidding(highest_player_, SchafkopfGameType(highest_game_));
}

void SchafkopfState::ApplyRufsauAction(int rufsau) {
    // Spieler has to find partner with Rufsau
    // Id for Aces: 7 (Schellen), 15 (Herz), 23 (Gras), 31 (Eichel)
    // Rufsau must not be on the own hand
    switch (rufsau) {
      case kSchellen:
        rufsau_ = 7;
      case kGras:
        rufsau_ = 23;
      case kEichel:
        rufsau_ = 31;
      default:
        rufsau_= -1;
    }

    switch (card_locations_[rufsau_]) {
      case kHand0:
        partner_ = 0;
      case kHand1:
        partner_ = 1;
      case kHand2:
        partner_ = 2;
      case kHand3:
        partner_ = 3;
    }

    // Error handling for selecting himself
    if (partner_ == spieler_) SpielFatalError("Spieler must not have the 'Rufsau' on his hand.");

    // Start game
    phase_ = kPlay;
    current_player_ = 0;
}

void SchafkopfState::ApplyPlayAction(int card) {
  SPIEL_CHECK_TRUE(card_locations_[card] == PlayerToLocation(current_player_));
  card_locations_[card] = kTrick;
  if (num_cards_played_ == 0) {
    CurrentTrick() = Trick(current_player_);
  }
  CurrentTrick().PlayCard(card);
  num_cards_played_++;
  if (num_cards_played_ % kNumPlayers == 0) {
    last_trick_winner_ = WinsTrick();
    current_player_ = last_trick_winner_;
    // When num_cards_played_ == kNumCards + kNumCardsInSchafkopf CurrentTrick() is
    // the same as PreviousTrick() and we don't want to overwrite it.
    if (num_cards_played_ < kNumCards - kNumCardsInSchafkopf) {
      CurrentTrick() = Trick(current_player_);
    }
    // Winner plays next card.
    if (last_trick_winner_ == solo_player_) {
        points_solo_ += PreviousTrick().Points();
        if (game_type_ == kNullGame) {
          // The solo player loses a Null game if they win any trick. The trick
          // they win could be without points so we add one to make sure ScoreUp
          // knows that the solo_player has won a trick.
          points_solo_++;
          phase_ = kGameOver;
          ScoreUp();
        }
    } else {
      points_team_ += PreviousTrick().Points();
    }
  } else {
      current_player_ = NextPlayer();
  }

  if (num_cards_played_ == kNumCards - kNumCardsInSchafkopf) {
    phase_ = kGameOver;
    ScoreUp();
  }
}

void SchafkopfState::ScoreUp() {
  if (game_type_ == kNullGame) {
    // Since we're using points as a reward we need to come up with some special
    // rule for Null.
    if (points_solo_ > 0) {
      points_solo_ = 30;
      points_team_ = 90;
    } else {
      points_solo_ = 90;
      points_team_ = 30;
    }
  } else {
    // Solo player gets the cards in the Schafkopf (unless Null is played).
    for (int card = 0; card < kNumCards; card++) {
      if (card_locations_[card] == kSchafkopf) {
        points_solo_ += CardValue(card);
      }
    }
  }
  for (int pl = 0; pl < kNumPlayers; ++pl) {
    if (solo_player_ == pl) {
      returns_[pl] = (points_solo_ - 60) / 120.0;
    } else {
      returns_[pl] = (points_team_ - 60) / 240.0;
    }
  }
}

std::string SchafkopfState::CardLocationsToString() const {
  std::string deck =  "Deck:     ";
  std::string hand0 = "Player 0: ";
  std::string hand1 = "Player 1: ";
  std::string hand2 = "Player 2: ";
  std::string schafkopf =  "Schafkopf:     ";
  for (int i = 0; i < kNumCards; i++) {
    switch (card_locations_[i]) {
      case kDeck:
        absl::StrAppendFormat(&deck, "%s ", ToCardSymbol(i));
        break;
      case kHand0:
        absl::StrAppendFormat(&hand0, "%s ", ToCardSymbol(i));
        break;
      case kHand1:
        absl::StrAppendFormat(&hand1, "%s ", ToCardSymbol(i));
        break;
      case kHand2:
        absl::StrAppendFormat(&hand2, "%s ", ToCardSymbol(i));
        break;
      case kSchafkopf:
        absl::StrAppendFormat(&schafkopf, "%s ", ToCardSymbol(i));
        break;
      default:
        break;
    }
  }
  return absl::StrFormat("%s\n%s\n%s\n%s\n%s\n",
                         deck, hand0, hand1, hand2, schafkopf);
}

std::vector<Action> SchafkopfState::LegalActions() const {
  switch (phase_) {
    case kDeal:
      return DealLegalActions();
    case kBidding:
      return BiddingLegalActions();
    case kDiscardCards:
      return DiscardCardsLegalActions();
    case kPlay:
      return PlayLegalActions();
    default:
      return {};
  }
}

std::vector<Action> SchafkopfState::DealLegalActions() const {
  std::vector<Action> legal_actions;
  legal_actions.reserve(kNumCards - history_.size());
  for (int i = 0; i < kNumCards; ++i) {
    if (card_locations_[i] == kDeck) legal_actions.push_back(i);
  }
  return legal_actions;
}

std::vector<Action> SchafkopfState::BiddingLegalActions() const {
  std::vector<Action> legal_actions;
  legal_actions.push_back(kBiddingActionBase + kPass);
  legal_actions.push_back(kBiddingActionBase + kDiamondsTrump);
  legal_actions.push_back(kBiddingActionBase + kHeartsTrump);
  legal_actions.push_back(kBiddingActionBase + kSpadesTrump);
  legal_actions.push_back(kBiddingActionBase + kClubsTrump);
  legal_actions.push_back(kBiddingActionBase + kGrand);
  legal_actions.push_back(kBiddingActionBase + kNullGame);
  return legal_actions;
}

std::vector<Action> SchafkopfState::DiscardCardsLegalActions() const {
  std::vector<Action> legal_actions;
  for (int card = 0; card < kNumCards; ++card) {
    if (card_locations_[card] == current_player_ + 1) {
      legal_actions.push_back(card);
    }
  }
  return legal_actions;
}

std::vector<Action> SchafkopfState::PlayLegalActions() const {
  std::vector<Action> legal_actions;
  legal_actions.reserve(kNumTricks - num_cards_played_ / kNumPlayers);
  if (num_cards_played_ % kNumPlayers != 0) {
    // Check if we can follow suit.
    int first_card = CurrentTrick().FirstCard();
    int suit = CardSuit(first_card);
    if (game_type_ == kNullGame) {
      for (int rank = 0; rank < kNumRanks; ++rank) {
        int card = static_cast<int>(suit) * kNumRanks + rank;
        if (card_locations_[card] == PlayerToLocation(current_player_)) {
          legal_actions.push_back(card);
        }
      }
    } else {
      // This is a bid fidely but it makes sure the legal actions are sorted
      // (which is required), which the special status of jacks makes hard
      // otherwise.
      for (int card = 0; card < kNumCards; ++card) {
        if ((IsTrump(first_card) && IsTrump(card)) ||
            (suit == CardSuit(card) &&
             CardRank(card) != kJack &&
             CardRank(first_card) != kJack)) {
          if (card_locations_[card] == PlayerToLocation(current_player_)) {
            legal_actions.push_back(card);
          }
        }
      }
    }
  }

  if (!legal_actions.empty()) {
    return legal_actions;
  }

  // Otherwise, we can play any of our cards.
  for (int card = 0; card < kNumCards; ++card) {
    if (card_locations_[card] == current_player_ + 1) {
      legal_actions.push_back(card);
    }
  }
  return legal_actions;
}

std::vector<std::pair<Action, double>> SchafkopfState::ChanceOutcomes() const {
  std::vector<std::pair<Action, double>> outcomes;
  int num_cards_remaining = kNumCards - history_.size();
  outcomes.reserve(num_cards_remaining);
  const double p_card = 1.0 / static_cast<double>(num_cards_remaining);
  for (int card = 0; card < kNumCards; ++card) {
    if (card_locations_[card] == kDeck) outcomes.emplace_back(card, p_card);
  }
  return outcomes;
}

void SchafkopfState::ObservationTensor(Player player,
                                  absl::Span<float> values) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, num_players_);

  std::fill(values.begin(), values.end(), 0.0);
  SPIEL_CHECK_EQ(values.size(), game_->ObservationTensorSize());
  if (phase_ == Phase::kDeal) return;
  auto ptr = values.begin();
  // Position:
  ptr[player] = 1;
  ptr += kNumPlayers;
  // Phase
  if (phase_ >= kBidding && phase_ <= kPlay) ptr[phase_ - kBidding] = 1;
  ptr += 3;
  // Players Cards
  for (int i = 0; i < kNumCards; ++i)
    if (card_locations_[i] == PlayerToLocation(player)) ptr[i] = 1;
  ptr += kNumCards;
  // All player bids.
  for (int i = 0; i < kNumPlayers; i++) {
    ptr[player_bids_[i]] = 1;
    ptr += kNumGameTypes;
  }
  // Who is the solo player.
  if (solo_player_ >= 0) ptr[solo_player_] = 1;
  ptr += kNumPlayers;
  // Information about the Schafkopf only for the solo_player_.
  if (player == solo_player_) {
    for (int i = 0; i < kNumCards; ++i)
      if (card_locations_[i] == kSchafkopf) ptr[i] = 1;
  }
  ptr += kNumCards;
  // Game type
  ptr[game_type_] = 1;
  ptr += kNumGameTypes;
  // Current trick
  if (phase_ == kPlay) {
    ptr[CurrentTrick().Leader()] = 1;
    ptr += kNumPlayers;
    const auto& cards = CurrentTrick().GetCards();
    for (int i = 0; i < kNumPlayers; i++) {
      if (cards.size() > i) ptr[cards[i]] = 1;
      ptr += kNumCards;
    }
  } else {
    ptr += kNumPlayers + kNumPlayers * kNumCards;
  }
  // Previous Trick
  if (CurrentTrickIndex() > 0) {
    ptr[PreviousTrick().Leader()] = 1;
    ptr += kNumPlayers;
    const auto& cards = PreviousTrick().GetCards();
    for (int i = 0; i < kNumPlayers; i++) {
      if (cards.size() > i) ptr[cards[i]] = 1;
      ptr += kNumCards;
    }
  } else {
    ptr += kNumPlayers + kNumPlayers * kNumCards;
  }
}

template <typename It>
std::vector<int> GetCardsFromMultiHot(It multi_hot) {
  std::vector<int> cards;
  for (int i = 0; i < kNumCards; i++) {
    if (multi_hot[i]) cards.push_back(i);
  }
  return cards;
}

template <typename It>
int GetIntFromOneHot(It one_hot, int num_values) {
  for (int i = 0; i < num_values; i++) {
    if (one_hot[i]) return i;
  }
  return -1;
}

std::string SchafkopfState::ObservationString(Player player) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, num_players_);
  // We construct the ObservationString from the ObservationTensor to give
  // some indication that the tensor representation is correct & complete.
  if (phase_ == Phase::kDeal) {
    return "No Observation";
  }
  std::vector<float> tensor(game_->ObservationTensorSize());
  ObservationTensor(player, absl::MakeSpan(tensor));
  std::string rv;
  auto ptr = tensor.begin();
  int player_pos = GetIntFromOneHot(ptr, kNumPlayers);
  absl::StrAppend(&rv, "PlPos:", player_pos);
  ptr += kNumPlayers;
  Phase phase = kDeal;
  if (ptr[0]) phase = kBidding;
  else if (ptr[1]) phase = kDiscardCards;
  else if (ptr[2]) phase = kPlay;
  else
    phase = kGameOver;
  absl::StrAppend(&rv, "|Phase:", PhaseToString(phase));
  ptr += 3;
  std::vector<int> player_cards = GetCardsFromMultiHot(ptr);
  absl::StrAppend(&rv, "|Hand:", CardsToString(player_cards));
  ptr += kNumCards;
  absl::StrAppend(&rv, "|Bids:");
  for (int i = 0; i < kNumPlayers; i++) {
    int player_bid = GetIntFromOneHot(ptr, kNumGameTypes);
    absl::StrAppend(
        &rv, SchafkopfGameTypeToString(static_cast<SchafkopfGameType>(player_bid)), " ");
    ptr += kNumGameTypes;
  }
  Player solo_player = GetIntFromOneHot(ptr, kNumPlayers);
  absl::StrAppend(&rv, "|SoloPl:", solo_player);
  ptr += kNumPlayers;
  std::vector<int> schafkopf_cards = GetCardsFromMultiHot(ptr);
  absl::StrAppend(&rv, "|Schafkopf:", CardsToString(schafkopf_cards));
  ptr += kNumCards;
  SchafkopfGameType game_type = SchafkopfGameType(GetIntFromOneHot(ptr, kNumGameTypes));
  absl::StrAppend(&rv, "|Game:", SchafkopfGameTypeToString(game_type));
  ptr += kNumGameTypes;
  Player current_trick_leader = GetIntFromOneHot(ptr, kNumPlayers);
  absl::StrAppend(&rv, "|CurrTrick(Leader:", current_trick_leader, "):");
  ptr += kNumPlayers;
  for (int i = 0; i < kNumPlayers; i++) {
    int card = GetIntFromOneHot(ptr, kNumCards);
    if (card >= 0) absl::StrAppend(&rv, ToCardSymbol(card), " ");
    ptr += kNumCards;
  }
  Player previous_trick_leader = GetIntFromOneHot(ptr, kNumPlayers);
  if (previous_trick_leader >= 0) {
    absl::StrAppend(&rv, "|PrevTrick(Leader:", previous_trick_leader, "):");
    ptr += kNumPlayers;
    for (int i = 0; i < kNumPlayers; i++) {
      int card = GetIntFromOneHot(ptr, kNumCards);
      if (card >= 0) absl::StrAppend(&rv, ToCardSymbol(card), " ");
      ptr += kNumCards;
    }
  }
  return rv;
}

// ********************************** SchafkopfGame *********************************

SchafkopfGame::SchafkopfGame(const GameParameters& params)
    : Game(kGameType, params) {}

std::unique_ptr<State> SchafkopfGame::NewInitialState() const {
  return std::unique_ptr<State>(new SchafkopfState(shared_from_this()));
}

}  // namespace schafkopf
}  // namespace open_spiel
