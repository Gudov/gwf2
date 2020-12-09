#pragma once

namespace GUI {
	bool isInputFullCapture();
	void setInputFullCapture(bool enable);

	bool isMouseEnabled();
	void setMouseEnabled(bool enable);

	bool isDemoWindow();
	bool isDumpWindow();

	void setupStyle();

	void overlayMenu();
};
