/*
 *  Copyright © 2017-2020 Wellington Wallace
 *
 *  This file is part of EasyEffects.
 *
 *  EasyEffects is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  EasyEffects is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with EasyEffects.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "equalizer_preset.hpp"
#include "util.hpp"

EqualizerPreset::EqualizerPreset()
    : input_settings_left(Gio::Settings::create("com.github.wwmm.easyeffects.equalizer.channel",
                                                "/com/github/wwmm/easyeffects/streaminputs/equalizer/leftchannel/")),
      input_settings_right(Gio::Settings::create("com.github.wwmm.easyeffects.equalizer.channel",
                                                 "/com/github/wwmm/easyeffects/streaminputs/equalizer/rightchannel/")),
      output_settings_left(Gio::Settings::create("com.github.wwmm.easyeffects.equalizer.channel",
                                                 "/com/github/wwmm/easyeffects/streamoutputs/equalizer/leftchannel/")),
      output_settings_right(
          Gio::Settings::create("com.github.wwmm.easyeffects.equalizer.channel",
                                "/com/github/wwmm/easyeffects/streamoutputs/equalizer/rightchannel/")) {
  input_settings = Gio::Settings::create("com.github.wwmm.easyeffects.equalizer",
                                         "/com/github/wwmm/easyeffects/streaminputs/equalizer/");

  output_settings = Gio::Settings::create("com.github.wwmm.easyeffects.equalizer",
                                          "/com/github/wwmm/easyeffects/streamoutputs/equalizer/");
}

void EqualizerPreset::save(boost::property_tree::ptree& root,
                           const std::string& section,
                           const Glib::RefPtr<Gio::Settings>& settings) {
  root.put(section + ".equalizer.mode", settings->get_string("mode"));

  const auto& nbands = settings->get_int("num-bands");

  root.put(section + ".equalizer.num-bands", nbands);

  root.put(section + ".equalizer.input-gain", settings->get_double("input-gain"));

  root.put(section + ".equalizer.output-gain", settings->get_double("output-gain"));

  root.put(section + ".equalizer.split-channels", settings->get_boolean("split-channels"));

  if (section == "input") {
    save_channel(root, "input.equalizer.left", input_settings_left, nbands);
    save_channel(root, "input.equalizer.right", input_settings_right, nbands);
  } else if (section == "output") {
    save_channel(root, "output.equalizer.left", output_settings_left, nbands);
    save_channel(root, "output.equalizer.right", output_settings_right, nbands);
  }
}

void EqualizerPreset::save_channel(boost::property_tree::ptree& root,
                                   const std::string& section,
                                   const Glib::RefPtr<Gio::Settings>& settings,
                                   const int& nbands) {
  for (int n = 0; n < nbands; n++) {
    root.put(section + ".band" + std::to_string(n) + ".type",
             settings->get_string(std::string("band" + std::to_string(n) + "-type")));

    root.put(section + ".band" + std::to_string(n) + ".mode",
             settings->get_string(std::string("band" + std::to_string(n) + "-mode")));

    root.put(section + ".band" + std::to_string(n) + ".slope",
             settings->get_string(std::string("band" + std::to_string(n) + "-slope")));

    root.put(section + ".band" + std::to_string(n) + ".solo",
             settings->get_boolean(std::string("band" + std::to_string(n) + "-solo")));

    root.put(section + ".band" + std::to_string(n) + ".mute",
             settings->get_boolean(std::string("band" + std::to_string(n) + "-mute")));

    root.put(section + ".band" + std::to_string(n) + ".gain",
             settings->get_double(std::string("band" + std::to_string(n) + "-gain")));

    root.put(section + ".band" + std::to_string(n) + ".frequency",
             settings->get_double(std::string("band" + std::to_string(n) + "-frequency")));

    root.put(section + ".band" + std::to_string(n) + ".q",
             settings->get_double(std::string("band" + std::to_string(n) + "-q")));
  }
}

void EqualizerPreset::load(const nlohmann::json& json,
                           const std::string& section,
                           const Glib::RefPtr<Gio::Settings>& settings) {
  update_key<double>(json.at(section).at("equalizer"), settings, "input-gain", "input-gain");

  update_key<double>(json.at(section).at("equalizer"), settings, "output-gain", "output-gain");

  update_string_key(json.at(section).at("equalizer"), settings, "mode", "mode");

  update_key<int>(json.at(section).at("equalizer"), settings, "num-bands", "num-bands");

  update_key<bool>(json.at(section).at("equalizer"), settings, "split-channels", "split-channels");

  const auto& nbands = settings->get_int("num-bands");

  if (section == std::string("input")) {
    load_channel(json.at(section).at("equalizer").at("left"), input_settings_left, nbands);
    load_channel(json.at(section).at("equalizer").at("right"), input_settings_right, nbands);
  } else if (section == std::string("output")) {
    load_channel(json.at(section).at("equalizer").at("left"), output_settings_left, nbands);
    load_channel(json.at(section).at("equalizer").at("right"), output_settings_right, nbands);
  }
}

void EqualizerPreset::load_channel(const nlohmann::json& json,
                                   const Glib::RefPtr<Gio::Settings>& settings,
                                   const int& nbands) {
  for (int n = 0; n < nbands; n++) {
    update_string_key(json.at("band" + std::to_string(n)), settings, "band" + std::to_string(n) + "-type", "type");

    update_string_key(json.at("band" + std::to_string(n)), settings, "band" + std::to_string(n) + "-mode", "mode");

    update_string_key(json.at("band" + std::to_string(n)), settings, "band" + std::to_string(n) + "-slope", "slope");

    update_key<bool>(json.at("band" + std::to_string(n)), settings, "band" + std::to_string(n) + "-solo", "solo");

    update_key<bool>(json.at("band" + std::to_string(n)), settings, "band" + std::to_string(n) + "-mute", "mute");

    update_key<double>(json.at("band" + std::to_string(n)), settings, "band" + std::to_string(n) + "-gain", "gain");

    update_key<double>(json.at("band" + std::to_string(n)), settings, "band" + std::to_string(n) + "-frequency",
                       "frequency");

    update_key<double>(json.at("band" + std::to_string(n)), settings, "band" + std::to_string(n) + "-q", "q");
  }
}

void EqualizerPreset::write(PresetType preset_type, boost::property_tree::ptree& root) {
  switch (preset_type) {
    case PresetType::output:
      save(root, "output", output_settings);
      break;
    case PresetType::input:
      save(root, "input", input_settings);
      break;
  }
}
