#include "plugin.hpp"
#include "SvgHelper.hpp"

struct Summons : Module {
	enum ParamId {
		ENUMS(KNOB_PARAMS, 5),
		PARAMS_LEN
	};
	enum InputId {
		CLK_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		ENUMS(STEP_OUTPUTS, 5),
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(PENTAGRAM_LIGHTS, 5),
		ENUMS(STEP_LIGHTS, 5),
		LIGHTS_LEN
	};

	Summons() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		for (auto i = 0; i < 5; i++) {
			configParam(KNOB_PARAMS + i,  -10.f, 10.f, 0, string::f("Step %d", i + 1), " V");
			configInput(CLK_INPUT, "Clock");
			configOutput(STEP_OUTPUTS + i, string::f("Step %d", i + 1));
			configLight(PENTAGRAM_LIGHTS + i);
			configLight(STEP_LIGHTS + i);
		}
	}

	void process(const ProcessArgs& args) override {
	}
};


struct SummonsWidget : ModuleWidget, SvgHelper<SummonsWidget> {
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

	for (auto i = 0; i < 5; i++) {
		bindParam<RoundBlackKnob>(string::f("knob%d", i + 1), Summons::KNOB_PARAMS + i);
		bindOutput<DarkPJ301MPort>(string::f("gate%d", i + 1), Summons::STEP_OUTPUTS + i);
		bindLight<MediumLight<RedLight>>(string::f("light%d", i + 1), Summons::PENTAGRAM_LIGHTS + i);
		bindLight<TinyLight<RedLight>>(string::f("steplight%d", i+1), Summons::STEP_LIGHTS + i);
	}

	bindInput<DarkPJ301MPort>("clk", Summons::CLK_INPUT);
}


Model* modelSummons = createModel<Summons, SummonsWidget>("summons");