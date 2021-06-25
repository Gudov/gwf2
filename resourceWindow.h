#pragma once
#include <string>

namespace GUI {
	namespace Resource {
		void draw();

		std::string getItemName();
		int getItemCount();

		void clearResourcesCache();
		void pushResource(std::string name);
		void updateResourcesFromCache();
	}
}