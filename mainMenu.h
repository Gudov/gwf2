#pragma once

#include <string>

namespace GUI {
	bool isInputFullCapture();
	void setInputFullCapture(bool enable);

	bool isMouseEnabled();
	void setMouseEnabled(bool enable);

	bool isDemoWindow();
	bool isDumpWindow();
	bool isProtoListWindow();
	bool isExecuteWindow();
	bool isResourceWindow();

	void setupStyle();

	void overlayMenu();

	bool getOptionStatus(std::string name);
	void addOption(std::string name);
};
