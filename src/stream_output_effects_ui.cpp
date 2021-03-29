/*
 *  Copyright © 2017-2020 Wellington Wallace
 *
 *  This file is part of PulseEffects.
 *
 *  PulseEffects is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PulseEffects is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PulseEffects.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "stream_output_effects_ui.hpp"

StreamOutputEffectsUi::StreamOutputEffectsUi(BaseObjectType* cobject,
                                             const Glib::RefPtr<Gtk::Builder>& refBuilder,
                                             StreamOutputEffects* soe_ptr,
                                             const std::string& schema)
    : Gtk::Box(cobject), EffectsBaseUi(refBuilder, soe_ptr, schema), soe(soe_ptr) {
  // populate stack

  //   auto b_pitch = Gtk::Builder::create_from_resource("/com/github/wwmm/pulseeffects/ui/pitch.glade");
  // auto b_crossfeed =
  //   Gtk::Builder::create_from_resource("/com/github/wwmm/pulseeffects/ui/crossfeed.glade");
  //   auto b_loudness = Gtk::Builder::create_from_resource("/com/github/wwmm/pulseeffects/ui/loudness.glade");
  //   auto b_multiband_gate =
  //   Gtk::Builder::create_from_resource("/com/github/wwmm/pulseeffects/ui/multiband_gate.glade"); auto b_deesser =
  //   Gtk::Builder::create_from_resource("/com/github/wwmm/pulseeffects/ui/deesser.glade"); auto b_stereo_tools =
  //   Gtk::Builder::create_from_resource("/com/github/wwmm/pulseeffects/ui/convolver.glade"); auto b_crystalizer =
  //   Gtk::Builder::create_from_resource("/com/github/wwmm/pulseeffects/ui/crystalizer.glade"); auto b_delay =
  //   Gtk::Builder::create_from_resource("/com/github/wwmm/pulseeffects/ui/rnnoise.glade");

  //   b_pitch->get_widget_derived("widgets_grid", pitch_ui, "com.github.wwmm.pulseeffects.pitch",
  //                               "/com/github/wwmm/pulseeffects/sinkinputs/pitch/");

  //   b_crossfeed->get_widget_derived("widgets_grid", crossfeed_ui, "com.github.wwmm.pulseeffects.crossfeed",
  //                                   "/com/github/wwmm/pulseeffects/sinkinputs/crossfeed/");

  //   b_loudness->get_widget_derived("widgets_grid", loudness_ui, "com.github.wwmm.pulseeffects.loudness",
  //                                  "/com/github/wwmm/pulseeffects/sinkinputs/loudness/");

  //   b_multiband_gate->get_widget_derived("widgets_grid", multiband_gate_ui,
  //   "com.github.wwmm.pulseeffects.multibandgate",
  //                                        "/com/github/wwmm/pulseeffects/sinkinputs/multibandgate/");

  //   b_deesser->get_widget_derived("widgets_grid", deesser_ui, "com.github.wwmm.pulseeffects.deesser",
  //                                 "/com/github/wwmm/pulseeffects/sinkinputs/deesser/");

  //   b_convolver->get_widget_derived("widgets_grid", convolver_ui, "com.github.wwmm.pulseeffects.convolver",
  //                                   "/com/github/wwmm/pulseeffects/sinkinputs/convolver/");

  //   b_crystalizer->get_widget_derived("widgets_grid", crystalizer_ui, "com.github.wwmm.pulseeffects.crystalizer",
  //                                     "/com/github/wwmm/pulseeffects/sinkinputs/crystalizer/");

  //   b_rnnoise->get_widget_derived("widgets_grid", rnnoise_ui, "com.github.wwmm.pulseeffects.rnnoise",
  //                                 "/com/github/wwmm/pulseeffects/sinkinputs/rnnoise/");

  //   stack->add(*pitch_ui, pitch_ui->name);
  //   stack->add(*crossfeed_ui, crossfeed_ui->name);
  //   stack->add(*loudness_ui, loudness_ui->name);
  //   stack->add(*multiband_gate_ui, multiband_gate_ui->name);
  //   stack->add(*deesser_ui, deesser_ui->name);
  //   stack->add(*convolver_ui, convolver_ui->name);
  //   stack->add(*crystalizer_ui, crystalizer_ui->name);
  //   stack->add(*rnnoise_ui, rnnoise_ui->name);

  // populate_listbox

  //   add_to_listbox(pitch_ui);
  //   add_to_listbox(crossfeed_ui);
  //   add_to_listbox(loudness_ui);
  //   add_to_listbox(multiband_gate_ui);
  //   add_to_listbox(deesser_ui);
  //   add_to_listbox(convolver_ui);
  //   add_to_listbox(crystalizer_ui);
  //   add_to_listbox(rnnoise_ui);

  level_meters_connections();
  up_down_connections();

  for (auto& node : pm->list_nodes) {
    if (node.media_class == "Stream/Output/Audio") {
      on_app_added(node);
    }
  }

  connections.emplace_back(soe->spectrum->power.connect(sigc::mem_fun(*spectrum_ui, &SpectrumUi::on_new_spectrum)));

  connections.emplace_back(
      soe->pm->stream_output_added.connect(sigc::mem_fun(*this, &StreamOutputEffectsUi::on_app_added)));
  connections.emplace_back(
      soe->pm->stream_output_changed.connect(sigc::mem_fun(*this, &StreamOutputEffectsUi::on_app_changed)));
  connections.emplace_back(
      soe->pm->stream_output_removed.connect(sigc::mem_fun(*this, &StreamOutputEffectsUi::on_app_removed)));

  connections.emplace_back(soe->pm->sink_changed.connect([&](auto nd_info) {
    if (nd_info.id == soe->pm->pe_sink_node.id) {
      std::ostringstream str;

      str << node_state_to_string(soe->pm->pe_sink_node.state) << std::string(5, ' ');

      str.precision(1);

      str << std::fixed << soe->pm->pe_sink_node.rate * 0.001F << " kHz" << std::string(5, ' ');

      device_state->set_text(str.str());
    }
  }));

  std::ostringstream str;

  str << node_state_to_string(soe->pm->pe_sink_node.state) << std::string(5, ' ');

  str.precision(1);

  str << std::fixed << soe->pm->pe_sink_node.rate * 0.001F << " kHz" << std::string(5, ' ');

  device_state->set_text(str.str());
}

StreamOutputEffectsUi::~StreamOutputEffectsUi() {
  util::debug(log_tag + "destroyed");
}

auto StreamOutputEffectsUi::add_to_stack(Gtk::Stack* stack, StreamOutputEffects* soe_ptr) -> StreamOutputEffectsUi* {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/pulseeffects/ui/effects_base.ui");

  auto* ui = Gtk::Builder::get_widget_derived<StreamOutputEffectsUi>(builder, "top_box", soe_ptr,
                                                                     "com.github.wwmm.pulseeffects.sinkinputs");

  auto stack_page = stack->add(*ui, "stream_output");

  stack_page->set_title(_("Output"));

  return ui;
}

void StreamOutputEffectsUi::level_meters_connections() {
  // global output level meter connection

  connections.emplace_back(
      soe->output_level->output_level.connect(sigc::mem_fun(*this, &StreamOutputEffectsUi::on_new_output_level_db)));

  //   // pitch level meters connections

  //   connections.emplace_back(soe->pitch_input_level.connect(sigc::mem_fun(*pitch_ui,
  //   &PitchUi::on_new_input_level_db)));
  //   connections.emplace_back(soe->pitch_output_level.connect(sigc::mem_fun(*pitch_ui,
  //   &PitchUi::on_new_output_level_db)));

  //   // crossfeed level meters connections

  //   connections.emplace_back(
  //       soe->crossfeed_input_level.connect(sigc::mem_fun(*crossfeed_ui, &CrossfeedUi::on_new_input_level_db)));
  //   connections.emplace_back(
  //       soe->crossfeed_output_level.connect(sigc::mem_fun(*crossfeed_ui, &CrossfeedUi::on_new_output_level_db)));

  //   // multiband_compressor level meters connections

  //   connections.emplace_back(soe->multiband_compressor->input_level.connect(
  //       sigc::mem_fun(*multiband_compressor_ui, &MultibandCompressorUi::on_new_input_level)));
  //   connections.emplace_back(soe->multiband_compressor->output_level.connect(
  //       sigc::mem_fun(*multiband_compressor_ui, &MultibandCompressorUi::on_new_output_level)));

  //   connections.emplace_back(soe->multiband_compressor->output0.connect(
  //       sigc::mem_fun(*multiband_compressor_ui, &MultibandCompressorUi::on_new_output0)));
  //   connections.emplace_back(soe->multiband_compressor->output1.connect(
  //       sigc::mem_fun(*multiband_compressor_ui, &MultibandCompressorUi::on_new_output1)));
  //   connections.emplace_back(soe->multiband_compressor->output2.connect(
  //       sigc::mem_fun(*multiband_compressor_ui, &MultibandCompressorUi::on_new_output2)));
  //   connections.emplace_back(soe->multiband_compressor->output3.connect(
  //       sigc::mem_fun(*multiband_compressor_ui, &MultibandCompressorUi::on_new_output3)));

  //   connections.emplace_back(soe->multiband_compressor->compression0.connect(
  //       sigc::mem_fun(*multiband_compressor_ui, &MultibandCompressorUi::on_new_compression0)));
  //   connections.emplace_back(soe->multiband_compressor->compression1.connect(
  //       sigc::mem_fun(*multiband_compressor_ui, &MultibandCompressorUi::on_new_compression1)));
  //   connections.emplace_back(soe->multiband_compressor->compression2.connect(
  //       sigc::mem_fun(*multiband_compressor_ui, &MultibandCompressorUi::on_new_compression2)));
  //   connections.emplace_back(soe->multiband_compressor->compression3.connect(
  //       sigc::mem_fun(*multiband_compressor_ui, &MultibandCompressorUi::on_new_compression3)));

  //   // loudness level meters connections

  //   connections.emplace_back(
  //       soe->loudness_input_level.connect(sigc::mem_fun(*loudness_ui, &LoudnessUi::on_new_input_level_db)));
  //   connections.emplace_back(
  //       soe->loudness_output_level.connect(sigc::mem_fun(*loudness_ui, &LoudnessUi::on_new_output_level_db)));

  //   // gate level meters connections

  //   connections.emplace_back(soe->gate_input_level.connect(sigc::mem_fun(*gate_ui, &GateUi::on_new_input_level_db)));
  //   connections.emplace_back(soe->gate_output_level.connect(sigc::mem_fun(*gate_ui,
  //   &GateUi::on_new_output_level_db))); connections.emplace_back(soe->gate->gating.connect(sigc::mem_fun(*gate_ui,
  //   &GateUi::on_new_gating)));

  //   // multiband_gate level meters connections

  //   connections.emplace_back(soe->multiband_gate->input_level.connect(
  //       sigc::mem_fun(*multiband_gate_ui, &MultibandGateUi::on_new_input_level)));
  //   connections.emplace_back(soe->multiband_gate->output_level.connect(
  //       sigc::mem_fun(*multiband_gate_ui, &MultibandGateUi::on_new_output_level)));

  //   connections.emplace_back(
  //       soe->multiband_gate->output0.connect(sigc::mem_fun(*multiband_gate_ui, &MultibandGateUi::on_new_output0)));
  //   connections.emplace_back(
  //       soe->multiband_gate->output1.connect(sigc::mem_fun(*multiband_gate_ui, &MultibandGateUi::on_new_output1)));
  //   connections.emplace_back(
  //       soe->multiband_gate->output2.connect(sigc::mem_fun(*multiband_gate_ui, &MultibandGateUi::on_new_output2)));
  //   connections.emplace_back(
  //       soe->multiband_gate->output3.connect(sigc::mem_fun(*multiband_gate_ui, &MultibandGateUi::on_new_output3)));

  //   connections.emplace_back(
  //       soe->multiband_gate->gating0.connect(sigc::mem_fun(*multiband_gate_ui, &MultibandGateUi::on_new_gating0)));
  //   connections.emplace_back(
  //       soe->multiband_gate->gating1.connect(sigc::mem_fun(*multiband_gate_ui, &MultibandGateUi::on_new_gating1)));
  //   connections.emplace_back(
  //       soe->multiband_gate->gating2.connect(sigc::mem_fun(*multiband_gate_ui, &MultibandGateUi::on_new_gating2)));
  //   connections.emplace_back(
  //       soe->multiband_gate->gating3.connect(sigc::mem_fun(*multiband_gate_ui, &MultibandGateUi::on_new_gating3)));

  //   // deesser level meters connections

  //   connections.emplace_back(
  //       soe->deesser_input_level.connect(sigc::mem_fun(*deesser_ui, &DeesserUi::on_new_input_level_db)));
  //   connections.emplace_back(
  //       soe->deesser_output_level.connect(sigc::mem_fun(*deesser_ui, &DeesserUi::on_new_output_level_db)));
  //   connections.emplace_back(
  //       soe->deesser->compression.connect(sigc::mem_fun(*deesser_ui, &DeesserUi::on_new_compression)));
  //   connections.emplace_back(soe->deesser->detected.connect(sigc::mem_fun(*deesser_ui,
  //   &DeesserUi::on_new_detected)));

  //   // convolver level meters connections

  //   connections.emplace_back(
  //       soe->convolver_input_level.connect(sigc::mem_fun(*convolver_ui, &ConvolverUi::on_new_input_level_db)));
  //   connections.emplace_back(
  //       soe->convolver_output_level.connect(sigc::mem_fun(*convolver_ui, &ConvolverUi::on_new_output_level_db)));

  //   // crystalizer level meters connections

  //   connections.emplace_back(
  //       soe->crystalizer_input_level.connect(sigc::mem_fun(*crystalizer_ui, &CrystalizerUi::on_new_input_level_db)));
  //   connections.emplace_back(
  //       soe->crystalizer_output_level.connect(sigc::mem_fun(*crystalizer_ui,
  //       &CrystalizerUi::on_new_output_level_db)));
  //   connections.emplace_back(
  //       soe->crystalizer->range_before.connect(sigc::mem_fun(*crystalizer_ui, &CrystalizerUi::on_new_range_before)));
  //   connections.emplace_back(
  //       soe->crystalizer->range_after.connect(sigc::mem_fun(*crystalizer_ui, &CrystalizerUi::on_new_range_after)));
}

void StreamOutputEffectsUi::up_down_connections() {
  //   connections.emplace_back(pitch_ui->plugin_up->signal_clicked().connect([=, this]() { on_up(pitch_ui); }));
  //   connections.emplace_back(pitch_ui->plugin_down->signal_clicked().connect([=, this]() { on_down(pitch_ui); }));

  //   connections.emplace_back(crossfeed_ui->plugin_up->signal_clicked().connect([=, this]() { on_up(crossfeed_ui);
  //   })); connections.emplace_back(crossfeed_ui->plugin_down->signal_clicked().connect([=, this]() {
  //   on_down(crossfeed_ui); }));

  //   connections.emplace_back(
  //       multiband_compressor_ui->plugin_up->signal_clicked().connect([=, this]() { on_up(multiband_compressor_ui);
  //       }));
  //   connections.emplace_back(
  //       multiband_compressor_ui->plugin_down->signal_clicked().connect([=, this]() {
  //       on_down(multiband_compressor_ui); }));

  //   connections.emplace_back(loudness_ui->plugin_up->signal_clicked().connect([=, this]() { on_up(loudness_ui); }));
  //   connections.emplace_back(loudness_ui->plugin_down->signal_clicked().connect([=, this]() { on_down(loudness_ui);
  //   }));

  //   connections.emplace_back(multiband_gate_ui->plugin_up->signal_clicked().connect([=, this]() {
  //   on_up(multiband_gate_ui);
  //   })); connections.emplace_back(
  //       multiband_gate_ui->plugin_down->signal_clicked().connect([=, this]() { on_down(multiband_gate_ui); }));

  //   connections.emplace_back(deesser_ui->plugin_up->signal_clicked().connect([=, this]() { on_up(deesser_ui); }));
  //   connections.emplace_back(deesser_ui->plugin_down->signal_clicked().connect([=, this]() { on_down(deesser_ui);
  //   }));

  //   connections.emplace_back(convolver_ui->plugin_up->signal_clicked().connect([=, this]() { on_up(convolver_ui);
  //   })); connections.emplace_back(convolver_ui->plugin_down->signal_clicked().connect([=, this]() {
  //   on_down(convolver_ui); }));

  //   connections.emplace_back(crystalizer_ui->plugin_up->signal_clicked().connect([=, this]() { on_up(crystalizer_ui);
  //   })); connections.emplace_back(crystalizer_ui->plugin_down->signal_clicked().connect([=, this]() {
  //   on_down(crystalizer_ui);
  //   }));

  //   connections.emplace_back(rnnoise_ui->plugin_up->signal_clicked().connect([=, this]() { on_up(rnnoise_ui); }));
  //   connections.emplace_back(rnnoise_ui->plugin_down->signal_clicked().connect([=, this]() { on_down(rnnoise_ui);
  //   }));
}
