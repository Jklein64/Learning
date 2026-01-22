#include "plugin.hpp"


struct Pentagram : Module {
	enum ParamId {
		KNOB1_PARAM,
		KNOB5_PARAM,
		KNOB2_PARAM,
		KNOB4_PARAM,
		KNOB3_PARAM,
		PROB_MOD_PARAM,
		PROB_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		RESET_INPUT,
		CLOCK_INPUT,
		PROB_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		CV_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Pentagram() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(KNOB1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(KNOB5_PARAM, 0.f, 1.f, 0.f, "");
		configParam(KNOB2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(KNOB4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(KNOB3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PROB_MOD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PROB_PARAM, 0.f, 1.f, 0.f, "");
		configInput(RESET_INPUT, "");
		configInput(CLOCK_INPUT, "");
		configInput(PROB_CV_INPUT, "");
		configOutput(CV_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct PentagramWidget : ModuleWidget {
	PentagramWidget(Pentagram* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Pentagram.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.64, 30.483)), module, Pentagram::KNOB1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.297, 53.981)), module, Pentagram::KNOB5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(72.983, 53.981)), module, Pentagram::KNOB2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.651, 92.003)), module, Pentagram::KNOB4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(60.629, 92.003)), module, Pentagram::KNOB3_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(41.14, 105.441)), module, Pentagram::PROB_MOD_PARAM));
		addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(56.676, 105.441)), module, Pentagram::PROB_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(67.331, 16.92)), module, Pentagram::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.722, 17.615)), module, Pentagram::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.397, 105.441)), module, Pentagram::PROB_CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(40.64, 64.489)), module, Pentagram::CV_OUTPUT));
	}
};


Model* modelPentagram = createModel<Pentagram, PentagramWidget>("Pentagram");