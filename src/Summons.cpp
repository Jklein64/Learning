#include "plugin.hpp"
#include "SvgHelper.hpp"

#include <iostream>

struct Summons : Module {
	enum ParamId {
		ENUMS(KNOB_PARAMS, 5),
		CHAOS_KNOB_PARAM,
		CHAOS_MOD_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CLK_INPUT,
		RST_INPUT,
		CHAOS_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		ENUMS(STEP_OUTPUTS, 5),
		CV_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(PENTAGRAM_LIGHTS, 5),
		ENUMS(STEP_LIGHTS, 5),
		LIGHTS_LEN
	};

	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;
	int index = 0;

	Summons() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(CLK_INPUT, "Clock");
		configInput(RST_INPUT, "Reset");
		configInput(CHAOS_CV_INPUT, "Chaos");
		configParam(CHAOS_KNOB_PARAM, 0.f, 1.f, 0.1f, "Chaos", "%", 0, 100);
		configParam(CHAOS_MOD_PARAM, 0.f, 1.f, 0.f, "Chaos CV", "%", 0, 100);
		configOutput(CV_OUTPUT, "CV");
		for (auto i = 0; i < 5; i++) {
			configParam(KNOB_PARAMS + i,  -10.f, 10.f, 0, string::f("Step %d", i + 1), " V");
			configOutput(STEP_OUTPUTS + i, string::f("Step %d", i + 1));
		}
	}

	void process(const ProcessArgs& args) override {
		bool reset = false;

		if (inputs[RST_INPUT].isConnected()) {
			// Constants copied from SEQ3.cpp
			reset = resetTrigger.process(inputs[RST_INPUT].getVoltage(), 0.1f, 2.f);
			if (reset) {
				index = 0;
			}
		}

		if (inputs[CLK_INPUT].isConnected()) {
			// Constants copied from SEQ3.cpp
			bool clock = clockTrigger.process(inputs[CLK_INPUT].getVoltage(), 0.1f, 2.f);
			// If clock and reset happen close together, ignore the clock
			if (clock && !reset) {
				auto chaosMod = params[CHAOS_MOD_PARAM].value;
				auto chaosCv = inputs[CHAOS_CV_INPUT].getVoltage();
				auto chaos = params[CHAOS_KNOB_PARAM].value + chaosMod * chaosCv;
				// Remap so p = 1 when chaos = 0, p = 1/3 when chaos = 1
				auto p = 1.f - 2.f / 3.f * rack::clamp(chaos);
				auto rand = random::uniform();
				if (rand < p) {
					index++; // circle path
				} else {
					// Decide an alternate path uniformly
					if (2 * rand < 1 - p) {
						index += 2; // closer pentagram path
					} else {
						index += 3; // farther pentagram path
					}
				}

				if (index >= 5) {
					index %= 5;
				}
			}
		}

		for (auto i = 0; i < 5; i++) {
			outputs[STEP_OUTPUTS + i].setVoltage((index == i) ? 10.f : 0.f);
			lights[STEP_LIGHTS + i].setBrightnessSmooth(index == i, args.sampleTime, 90.f);
			lights[PENTAGRAM_LIGHTS + i].setBrightnessSmooth(3.f * (index == i), args.sampleTime, 10.f);
		}

		outputs[CV_OUTPUT].setVoltage(params[KNOB_PARAMS + index].value);
	}
};


struct SummonsWidget : ModuleWidget, SvgHelper<SummonsWidget> {
	std::array<int, 15> transitionIds = { 12, 23, 34, 45, 51, 13, 24, 35, 41, 52, 14, 25, 31, 42, 53 };
	std::array<NSVGshape*, 15> transitionShapes;

	SummonsWidget(Summons* module) {
		setModule(module);
		// Enable development features
		setDevMode(true);  
		load();
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

void SummonsWidget::load() {
	loadPanel(asset::plugin(pluginInstance, "res/Summons.svg"));

	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	// Load transitions by ID
	for (size_t i = 0; i < transitionIds.size(); i++) {
		int id = transitionIds[i];
		if (auto* shape = findNamed(string::f("c%d", id))) {
			shape->opacity = 0.5f;
			shape->flags |= NSVG_FLAGS_VISIBLE;
			transitionShapes[i] = shape;
		}
	}

	for (auto i = 0; i < 5; i++) {
		bindParam<RoundBlackKnob>(string::f("knob%d", i + 1), Summons::KNOB_PARAMS + i);
		bindOutput<DarkPJ301MPort>(string::f("gate%d", i + 1), Summons::STEP_OUTPUTS + i);
		bindLight<MediumLight<RedLight>>(string::f("light%d", i + 1), Summons::PENTAGRAM_LIGHTS + i);
		bindLight<TinyLight<RedLight>>(string::f("steplight%d", i+1), Summons::STEP_LIGHTS + i);
	}

	bindParam<Trimpot>("chaosmod", Summons::CHAOS_MOD_PARAM);
	bindParam<RoundBigBlackKnob>("chaosknob", Summons::CHAOS_KNOB_PARAM);
	bindInput<DarkPJ301MPort>("clk", Summons::CLK_INPUT);
	bindInput<DarkPJ301MPort>("rst", Summons::RST_INPUT);
	bindInput<DarkPJ301MPort>("chaoscv", Summons::CHAOS_CV_INPUT);
	bindOutput<DarkPJ301MPort>("cv", Summons::CV_OUTPUT);
}


Model* modelSummons = createModel<Summons, SummonsWidget>("summons");