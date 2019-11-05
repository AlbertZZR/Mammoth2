#include "glviewer.h"
#include "pcl_header.h"

using namespace glviewer;

namespace mammoth{
	template<class T>
    class MPointViewer{
    public:
		MPointViewer() {
			p_glviewer = nullptr;
			p_textnode = nullptr;
		}
        void init(){
            if (p_glviewer == nullptr) {
                p_glviewer = GetGLDevice(0, NULL, "Point Cloud Window");
            }
			++window_count;
			p_glviewer->SetParam(DeviceParams::Wave_ZPlane, -1.8f);
			p_glviewer->SetParam(DeviceParams::POINT_SIZE, 2.0f);
			//unsigned char b[64] = { 0xFF,0x4B,0x9B,0x3B,0x8D,0xF7,0x7C,0x3F,0xC7,0x12,0x1D,0xBE,0x0B,0xDD,0x40,0x3A,0x4F,0x12,0x1D,0x3E,0x39,0xF8,0x7C,0x3F,0xED,0x8A,0x93,0x3D,0x18,0x12,0x9C,0x40,0x81,0x21,0xF0,0xBF,0x00,0x00,0x80,0x40,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC };
			//p_glviewer->SetCameraData((CameraData *)b);
			//p_glviewer->AddWavePlane(2, 0, 50);
			//p_glviewer->SetOnPickingCallback(selectResultHandle);
			//p_glviewer->RegisterCallback(' ', key_pressed);
			//init text
			for (int i = 0; i < 17; i++) {
				text_ids.push_back(add_text("0", 0, 0, 0.4f, { 0.0f, 0.0f, 0.0f, 0.0f }));
			}
        }
        void set_cloud(typename pcl::PointCloud<T>::Ptr cloud){
            p_glviewer->SetPointCloud(&((*cloud)[0]), cloud->size());
        }

		void set_text(size_t id, std::string str, int start_x, int start_y, float scale_rate, Color4F color) {
			if (p_textnode == nullptr) {
				p_textnode = p_glviewer->CreateTextNode();
			}
			if (id >= text_ids.size()) {
				id = text_ids.size() - 1;
			}
			p_textnode->UpdateText(text_ids[id], str.c_str());
			p_textnode->UpdateColor(text_ids[id], color);
			p_textnode->UpdatePosition(text_ids[id], start_x, start_y);
		}

		size_t add_text(std::string str, int start_x, int start_y, float scale_rate, Color4F color) {
			if (p_textnode == nullptr) {
				p_textnode = p_glviewer->CreateTextNode();
			}
			size_t id = p_textnode->AddText(str.c_str(), start_x, start_y, scale_rate, color);
			return id;
		}

    private:
		static int window_count;
		static std::vector<size_t> text_ids;
        GLDevice* p_glviewer;
		TextNode* p_textnode;
	};
}

template<class T>
std::vector<size_t> mammoth::MPointViewer<T>::text_ids;

template<class T>
int mammoth::MPointViewer<T>::window_count = 0;