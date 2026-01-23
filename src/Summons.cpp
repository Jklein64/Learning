#include "plugin.hpp"
#include "SvgHelper.hpp"

struct Summons : Module {
	enum ParamId {
		ENUMS(KNOB_PARAMS, 5),
		PROB_PARAM,
		PROB_MOD_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CLK_INPUT,
		RST_INPUT,
		PROB_CV_INPUT,
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

	Summons() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(CLK_INPUT, "Clock");
		configInput(RST_INPUT, "Reset");
		configInput(PROB_CV_INPUT, "Probability");
		configParam(PROB_PARAM, 0.f, 1.f, 0.1f, "Probability");
		configParam(PROB_MOD_PARAM, 0.f, 1.f, 0.f, "Probability CV", "%", 0, 100);
		configOutput(CV_OUTPUT, "CV");
		for (auto i = 0; i < 5; i++) {
			configParam(KNOB_PARAMS + i,  -10.f, 10.f, 0, string::f("Step %d", i + 1), " V");
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

	bindParam<Trimpot>("probmod", Summons::PROB_MOD_PARAM);
	bindParam<RoundBigBlackKnob>("probknob", Summons::PROB_PARAM);
	bindInput<DarkPJ301MPort>("clk", Summons::CLK_INPUT);
	bindInput<DarkPJ301MPort>("rst", Summons::RST_INPUT);
	bindInput<DarkPJ301MPort>("probcv", Summons::PROB_CV_INPUT);
	bindOutput<DarkPJ301MPort>("cv", Summons::CV_OUTPUT);
}


Model* modelSummons = createModel<Summons, SummonsWidget>("summons");