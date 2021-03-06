/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "hero/TreasureState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "lowlevel/Sound.h"
#include "lua/LuaContext.h"
#include "EquipmentItem.h"
#include "Game.h"
#include "Map.h"

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 * \param treasure The treasure to give to the hero.
 * \param callback_ref Lua ref to a function to call when the
 * treasure's dialog finishes (possibly LUA_REFNIL).
 */
Hero::TreasureState::TreasureState(
    Hero& hero,
    const Treasure& treasure,
    int callback_ref):

  State(hero, "treasure"),
  treasure(treasure),
  callback_ref(callback_ref) {

}

/**
 * \brief Destructor.
 */
Hero::TreasureState::~TreasureState() {

}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::TreasureState::start(State* previous_state) {

  State::start(previous_state);

  // Show the animation.
  get_sprites().save_animation_direction();
  get_sprites().set_animation_brandish();

  // Play the sound.
  const std::string& sound_id = treasure.get_item().get_sound_when_brandished();
  if (!sound_id.empty()) {
    Sound::play(sound_id);
  }

  // Give the treasure.
  treasure.give_to_player();

  // Show a dialog (Lua does the job after this).
  int callback_ref = this->callback_ref;
  this->callback_ref = LUA_REFNIL;
  get_lua_context().notify_hero_brandish_treasure(treasure, callback_ref);
}

/**
 * \brief Stops this state.
 * \param next_state the next state
 */
void Hero::TreasureState::stop(State* next_state) {

  State::stop(next_state);

  // restore the sprite's direction
  get_sprites().restore_animation_direction();
  get_lua_context().cancel_callback(callback_ref);
  callback_ref = LUA_REFNIL;
}

/**
 * \brief Draws this state.
 */
void Hero::TreasureState::draw_on_map() {

  State::draw_on_map();

  const Hero& hero = get_hero();
  int x = hero.get_x();
  int y = hero.get_y();

  const Rectangle &camera_position = get_map().get_camera_position();
  treasure.draw(get_map().get_visible_surface(),
      x - camera_position.get_x(),
      y - 24 - camera_position.get_y());
}

/**
 * \brief Returns the action to do with an item previously carried by the hero when this state starts.
 * \param carried_item the item carried in the previous state
 * \return the action to do with a previous carried item when this state starts
 */
CarriedItem::Behavior Hero::TreasureState::get_previous_carried_item_behavior(
    CarriedItem& carried_item) {
  return CarriedItem::BEHAVIOR_DESTROY;
}

/**
 * \brief Returns whether the hero is brandishing a treasure in this state.
 * \return \c true if the hero is brandishing a treasure.
 */
bool Hero::TreasureState::is_brandishing_treasure() const {
  return true;
}

