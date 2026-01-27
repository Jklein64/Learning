#include "plugin.hpp"
#include "SvgHelper.hpp"

struct Transition {
	// One-indexed two-digit transition ID
	int index, a, b;

	std::string id() {
		return string::f("c%d%d", a, b);
	}

	static Transition from(int a, int b) {
		int index = -1;
		switch(10 * a + b) {
			// Inner
			case 12:
			case 21: index = 0; break;
			case 23: 
			case 32: index = 1; break; 
			case 34:
			case 43: index = 2; break;
			case 45:
			case 54: index = 3; break;
			case 51:
			case 15: index = 4; break;
			// Outer
			case 13:
			case 31: index = 5; break;
			case 24:
			case 42: index = 6; break;
			case 35:
			case 53: index = 7; break;
			case 41:
			case 14: index = 8; break;
			case 52:
			case 25: index = 9; break;
		}

		if (index < 0) {
			WARN("Could not find index for transition from %d to %d", a, b);
			return Transition { 0, 1, 2 };
		}

		return Transition { index, a, b };
	}
};

static constexpr std::array<Transition, 10> transitions = {{ 
	{ 0, 1, 2 }, { 1, 2, 3 }, { 2, 3, 4 }, { 3, 4, 5 }, { 4, 5, 1 }, // outer
	{ 5, 1, 3 }, { 6, 2, 4 }, { 7, 3, 5 }, { 8, 4, 1 }, { 9, 5, 2 }  // inner
}};

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
		ENUMS(TRANSITION_LIGHTS, 10),
		LIGHTS_LEN
	};

	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;
	int index = 0;
	std::array<float, transitions.size()> brightnesses;

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

		for (size_t i = 0; i < transitions.size(); i++) {
			lights[TRANSITION_LIGHTS + i].setBrightness(0.f);
			brightnesses[i] = 0.f;
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
				int a = index + 1;
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

				auto t = Transition::from(a, index + 1);
				if (t.index >= 0) {
					for (size_t i = 0; i < transitions.size(); i++) {
						brightnesses[i] = 3.f * (t.index == static_cast<int>(i));
					}
				}
			}

		}

		for (auto i = 0; i < 5; i++) {
			outputs[STEP_OUTPUTS + i].setVoltage((index == i) ? 10.f : 0.f);
			lights[STEP_LIGHTS + i].setBrightnessSmooth(index == i, args.sampleTime, 90.f);
			lights[PENTAGRAM_LIGHTS + i].setBrightnessSmooth(3.f * (index == i), args.sampleTime, 10.f);
		}

		for (size_t i = 0; i < transitions.size(); i++) {
			// Hold just long enough to consistently see pentagram at ~180 BPM with 100% chaos
			lights[TRANSITION_LIGHTS + i].setBrightnessSmooth(brightnesses[i], args.sampleTime, 1.75f);
		}

		outputs[CV_OUTPUT].setVoltage(params[KNOB_PARAMS + index].value);
	}
};

struct SummonsWidget : ModuleWidget, SvgHelper<SummonsWidget> {
	std::array<SvgWidget*, 10> transitionWidgets;

	SummonsWidget(Summons* module) {
		setModule(module);
		// Enable development features
		setDevMode(true);
		// Otherwise they're uninitialized
		transitionWidgets.fill(nullptr);
		load();
	}

	void load();

	void step() override {
		if (auto* moduleSummons = static_cast<Summons*>(module)) {
			for (size_t i = 0; i < transitionWidgets.size(); i++) {
				if (auto* widget = transitionWidgets[i]) {
					assert(moduleSummons != nullptr);
					auto& light = moduleSummons->lights[Summons::TRANSITION_LIGHTS + i];
					// Assumes that this is only for transition widgets, which have exactly one shape
					widget->svg->handle->shapes[0].opacity = rack::clamp(light.getBrightness());
				}
			}
		}

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
	for (Transition t : transitions) {
		if (auto* shape = findNamed(t.id())) {
			float x = shape->bounds[0], y = shape->bounds[1];
			float w = shape->bounds[2] - x, h = shape->bounds[3] - y;
			auto* widget = createWidget<SvgWidget>(Vec(0, 0));
			auto svg = std::make_shared<window::Svg>();
			auto* image = new NSVGimage();
			image->width = w;
			image->height = h;
			// Clone the shape for modification
			auto* newShape = new NSVGshape(*shape);
			// Duplicate paths since that's a nested pointer and copy ctor is shallow
			NSVGpath* head = NULL;
			NSVGpath* prev = NULL;
			NSVGpath* curr = shape->paths;
			while (curr != NULL) {
				auto* dupe = nsvgDuplicatePath(curr);
				if (prev == NULL) {
					head = dupe;
				} else {
					prev->next = dupe;
				}
				prev = dupe;
				curr = curr->next;
			}
			newShape->paths = head;
			// NSVGimage.shape is a linked list of all the shapes, so set the next 
			// shape to NULL (not nullptr because it's C) to only use the one we want
			newShape->next = NULL;
			// Make visible (nanosvg turns some attributes into flags)
			newShape->flags |= NSVG_FLAGS_VISIBLE;
			image->shapes = newShape;
			svg->handle = image;
			widget->setSvg(svg);
			addChild(widget);
			widget->setVisible(true);
			// Make transparent to start; SVG is always one shape
			widget->svg->handle->shapes[0].opacity = 0.f;
			// Add widget to local array (assumes i is always a valid index)
			if (transitionWidgets[t.index] != nullptr) {
				DEBUG("Overwriting widget at index %d, ptr = %x", t.index, transitionWidgets[t.index]);
				delete transitionWidgets[t.index];
			}
			transitionWidgets[t.index] = widget;
			DEBUG("Added transition \"%s\" at index %d", t.id().c_str(), t.index);
		} else {
			WARN("Unable to find shape in svg with id \"%s\"", t.id().c_str());
		}
	}

	for (auto i = 0; i < 5; i++) {
		bindParam<RoundBlackKnob>(string::f("knob%d", i + 1), Summons::KNOB_PARAMS + i);
		bindOutput<DarkPJ301MPort>(string::f("gate%d", i + 1), Summons::STEP_OUTPUTS + i);
		bindLight<MediumLight<RedLight>>(string::f("light%d", i + 1), Summons::PENTAGRAM_LIGHTS + i);
		bindLight<TinyLight<RedLight>>(string::f("steplight%d", i + 1), Summons::STEP_LIGHTS + i);
	}

	bindParam<Trimpot>("chaosmod", Summons::CHAOS_MOD_PARAM);
	bindParam<RoundBigBlackKnob>("chaosknob", Summons::CHAOS_KNOB_PARAM);
	bindInput<DarkPJ301MPort>("clk", Summons::CLK_INPUT);
	bindInput<DarkPJ301MPort>("rst", Summons::RST_INPUT);
	bindInput<DarkPJ301MPort>("chaoscv", Summons::CHAOS_CV_INPUT);
	bindOutput<DarkPJ301MPort>("cv", Summons::CV_OUTPUT);
}


Model* modelSummons = createModel<Summons, SummonsWidget>("summons");