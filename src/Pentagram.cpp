#include "plugin.hpp"
#include "SvgHelper.hpp"

struct Pentagram : Module {
	enum ParamId {
		KNOB1_PARAM,
		KNOB2_PARAM,
		KNOB3_PARAM,
		KNOB4_PARAM,
		KNOB5_PARAM,
		// KNOB1_PARAM,
		// KNOB2_PARAM,
		// KNOB3_PARAM,
		// KNOB4_PARAM,
		// KNOB5_PARAM,
		// PROB_PARAM,
		// PROB_MOD_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		// CLOCK_INPUT,
		// RESET_INPUT,
		// PROB_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		// CV_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Pentagram() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(KNOB1_PARAM, -10.f, 10.f, 0, "step 1");
		configParam(KNOB2_PARAM, -10.f, 10.f, 0, "step 2");
		configParam(KNOB3_PARAM, -10.f, 10.f, 0, "step 3");
		configParam(KNOB4_PARAM, -10.f, 10.f, 0, "step 4");
		configParam(KNOB5_PARAM, -10.f, 10.f, 0, "step 5");
		// configParam(KNOB1_PARAM, 0.f, 1.f, 0.f, "");
		// configParam(KNOB5_PARAM, 0.f, 1.f, 0.f, "");
		// configParam(KNOB2_PARAM, 0.f, 1.f, 0.f, "");
		// configParam(KNOB4_PARAM, 0.f, 1.f, 0.f, "");
		// configParam(KNOB3_PARAM, 0.f, 1.f, 0.f, "");
		// configParam(PROB_MOD_PARAM, 0.f, 1.f, 0.f, "");
		// configParam(PROB_PARAM, 0.f, 1.f, 0.f, "");
		// configInput(RESET_INPUT, "");
		// configInput(CLOCK_INPUT, "");
		// configInput(PROB_CV_INPUT, "");
		// configOutput(CV_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct PentagramWidget : ModuleWidget, SvgHelper<PentagramWidget> {
	PentagramWidget(Pentagram* module) {
		setModule(module);
		// Enable development features
		setDevMode(true);  
		load();

		// addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.64, 30.483)), module, Pentagram::KNOB1_PARAM));
		// addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.297, 53.981)), module, Pentagram::KNOB5_PARAM));
		// addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(72.983, 53.981)), module, Pentagram::KNOB2_PARAM));
		// addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.651, 92.003)), module, Pentagram::KNOB4_PARAM));
		// addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(60.629, 92.003)), module, Pentagram::KNOB3_PARAM));
		// addParam(createParamCentered<Trimpot>(mm2px(Vec(41.14, 105.441)), module, Pentagram::PROB_MOD_PARAM));
		// addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(56.676, 105.441)), module, Pentagram::PROB_PARAM));

		// addInput(createInputCentered<PJ301MPort>(mm2px(Vec(67.331, 16.92)), module, Pentagram::RESET_INPUT));
		// addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.722, 17.615)), module, Pentagram::CLOCK_INPUT));
		// addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.397, 105.441)), module, Pentagram::PROB_CV_INPUT));

		// addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(40.64, 64.489)), module, Pentagram::CV_OUTPUT));
	}

	void load();

	void step() override {
        ModuleWidget::step();
        SvgHelper::step();
    }

	void appendContextMenu(Menu* menu) override {
		SvgHelper::appendContextMenu(menu);
	}
};

void PentagramWidget::load() {
	loadPanel(asset::plugin(pluginInstance, "res/Pentagram.svg"));

	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	// bindInput<PJ301MPort>("clock", Pentagram::CLOCK_INPUT);
	bindParam<RoundSmallBlackKnob>("knob1", Pentagram::KNOB1_PARAM);
	bindParam<Davies1900hBlackKnob>("knob2", Pentagram::KNOB2_PARAM);
	bindParam<BefacoTinyKnob>("knob3", Pentagram::KNOB3_PARAM);
	bindParam<Trimpot>("knob4", Pentagram::KNOB4_PARAM);
	bindParam<RoundBlackKnob>("knob5", Pentagram::KNOB5_PARAM);
}


Model* modelPentagram = createModel<Pentagram, PentagramWidget>("Pentagram");