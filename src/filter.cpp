/*
 *  Copyright © 2017-2023 Wellington Wallace
 *
 *  This file is part of Easy Effects.
 *
 *  Easy Effects is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Easy Effects is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Easy Effects. If not, see <https://www.gnu.org/licenses/>.
 */

#include "filter.hpp"

Filter::Filter(const std::string& tag,
               const std::string& schema,
               const std::string& schema_path,
               PipeManager* pipe_manager)
    : PluginBase(tag, tags::plugin_name::filter, tags::plugin_package::calf, schema, schema_path, pipe_manager) {
  lv2_wrapper = std::make_unique<lv2::Lv2Wrapper>("http://calf.sourceforge.net/plugins/Filter");

  package_installed = lv2_wrapper->found_plugin;

  if (!package_installed) {
    util::debug(log_tag + "http://calf.sourceforge.net/plugins/Filter is not installed");
  }

  lv2_wrapper->bind_key_double<"freq", "frequency">(settings);

  lv2_wrapper->bind_key_double_db<"res", "resonance">(settings);

  lv2_wrapper->bind_key_enum<"mode", "mode">(settings);

  setup_input_output_gain();

  // g_timeout_add_seconds(1, GSourceFunc(+[](Filter* self) {
  //                         if (!self->lv2_wrapper->has_ui()) {
  //                           self->lv2_wrapper->load_ui();
  //                         } else {
  //                           self->lv2_wrapper->notify_ui();
  //                           self->lv2_wrapper->update_ui();
  //                         }

  //                         return 1;
  //                       }),
  //                       this);
}

Filter::~Filter() {
  if (connected_to_pw) {
    disconnect_from_pw();
  }

  util::debug(log_tag + name + " destroyed");
}

void Filter::setup() {
  if (!lv2_wrapper->found_plugin) {
    return;
  }

  lv2_wrapper->set_n_samples(n_samples);

  if (lv2_wrapper->get_rate() != rate) {
    lv2_wrapper->create_instance(rate);
  }
}

void Filter::process(std::span<float>& left_in,
                     std::span<float>& right_in,
                     std::span<float>& left_out,
                     std::span<float>& right_out) {
  if (!lv2_wrapper->found_plugin || !lv2_wrapper->has_instance() || bypass) {
    std::copy(left_in.begin(), left_in.end(), left_out.begin());
    std::copy(right_in.begin(), right_in.end(), right_out.begin());

    return;
  }

  if (input_gain != 1.0F) {
    apply_gain(left_in, right_in, input_gain);
  }

  lv2_wrapper->connect_data_ports(left_in, right_in, left_out, right_out);
  lv2_wrapper->run();

  if (output_gain != 1.0F) {
    apply_gain(left_out, right_out, output_gain);
  }

  if (post_messages) {
    get_peaks(left_in, right_in, left_out, right_out);

    if (send_notifications) {
      notify();
    }
  }
}

auto Filter::get_latency_seconds() -> float {
  return 0.0F;
}
