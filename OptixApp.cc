#include "OptixApp.h"

#include <fstream>
static std::vector<char> __optix_app_read_file(const std::string &filename, bool terminate) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
	    std::cerr << "failed to open file!" << std::endl;
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	if (terminate) buffer.push_back(0);
	return buffer;
}

OptixApp::OptixApp() {
	load_programs();
	texture_push_count = 0;
}


OptixApp::~OptixApp() {
	context->destroy();
}

size_t OptixApp::create_perspective(unsigned width, unsigned height, RTCamera* camera)
{
    Perspective p;
    p.in_medium = false;
	size_t i = perspectives.size();
    glGenBuffers(1, &p.gl_buf);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, p.gl_buf);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * sizeof(float) * 4, (void*) 0, GL_STREAM_READ);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    std::cout << "gl_tex_id = " << p.gl_buf << std::endl;

	p.width = width;
	p.height = height;
	p.image = context->createBufferFromGLBO(RT_BUFFER_OUTPUT, p.gl_buf);
	p.image->setFormat(RT_FORMAT_FLOAT4);
	p.image->setSize(width, height);
	p.camera = camera;

	p.light_intensity = 0.0;
	p.light_position = {0.0, 0.0, 0.0};

	perspectives.push_back(p);
	return i;
}

static void callbackUsageReport(int level, const char* tag, const char* msg, void* cbdata){
    std::cout << "[" << level << "][" << std::left << std::setw(12) << tag << "] " << msg;
}

void OptixApp::load_programs()
{
	try {
		context = optix::Context::create();

		context->setRayTypeCount(2);
		context->setEntryPointCount(1);

        context->setStackSize(1024*8);

        context->setPrintEnabled(true);
        context->setExceptionEnabled(RT_EXCEPTION_ALL, true);
        //context->setUsageReportCallback(callbackUsageReport, 3, NULL);

        for(auto &entry : std::filesystem::directory_iterator(".")){
            if(entry.path().generic_string().find(".cu.ptx")!=std::string::npos){
                std::string program_name = entry.path().generic_string();
                program_name.erase(program_name.find(".cu.ptx"));
                int underline_count = 0;
                for(size_t i = 0; i < program_name.size(); i++){
                    if(program_name[i] == '_'){
                        underline_count++;
                        if(underline_count >= 5){
                            program_name.erase(0, i+1);
                            break;
                        }
                    }
                }
                std::cout << "loading program: " << program_name << std::endl;
                auto program_str = __optix_app_read_file(entry.path().generic_string(), true);
                optix::Program program = context->createProgramFromPTXString(program_str.data(), program_name);
                program_space[program_name] = program;
            }
        }
   
		context->setRayGenerationProgram(0, program_space["raygen"]);
		context->setMissProgram(0, program_space["miss"]);
		context->setExceptionProgram(0, program_space["exception"]);

		context->setMaxCallableProgramDepth(10);

		int RTX = true; // try once with true then false to see performance difference
		if (rtGlobalSetAttribute(RT_GLOBAL_ATTRIBUTE_ENABLE_RTX, sizeof(RTX), &RTX) != RT_SUCCESS)
			printf("Error setting RTX mode. \n");
		else
			printf("OptiX RTX execution mode is %s.\n", (RTX) ? "on" : "off");

		root_group = context->createGroup();
		root_group->setAcceleration(context->createAcceleration("Trbvh"));

		context["sysTopObject"]->set(root_group);

	}
	catch (optix::Exception& e)
	{
		std::cerr << e.getErrorString() << std::endl;
		system("PAUSE");
		exit(2);
	}
}

void OptixApp::render_perspective(size_t i)
{
	try {
		//if(texture_push_count > 0) std::cout << "texture_push_count = " << texture_push_count/1e6 << std::endl;
		if (i >= perspectives.size()) return;
		if (perspectives[i].camera == nullptr) return;
		glm::vec3 pos = perspectives[i].camera->get_position();

		context["origin"]->setFloat(pos.x, pos.y, pos.z);
		context["view"]->setMatrix3x3fv(false, glm::value_ptr(glm::mat3(perspectives[i].camera->get_view(false))));
		context["focus"]->setFloat(perspectives[i].camera->get_focus());
		context["result_buffer"]->set(perspectives[i].image);

        context["light_position"]->setFloat(perspectives[i].light_position.x, perspectives[i].light_position.y, perspectives[i].light_position.z);
        context["light_intensity"]->setFloat(perspectives[i].light_intensity);

		context["in_medium"]->setInt(perspectives[i].in_medium);
        context["medium_color"]->setFloat(perspectives[i].medium_color.r, perspectives[i].medium_color.g, perspectives[i].medium_color.b);
        context["medium_density"]->setFloat(perspectives[i].medium_density);
        context["ground_radius"]->setFloat(perspectives[i].ground_radius);
        context["atmosphere_radius"]->setFloat(perspectives[i].atmosphere_radius);
        context["rayleigh"]->setFloat(perspectives[i].rayleigh);
        context["mia"]->setFloat(perspectives[i].mia);

		context->validate();
		//std::cout << "Launching context " << i << " -> " << perspectives[i].width << " - " << perspectives[i].height << std::endl;
		context->launch(0, perspectives[i].width, perspectives[i].height);
		texture_push_count = 0;
	}
	catch (optix::Exception& e)
	{
		std::cerr << e.getErrorString() << std::endl;
	}
}

void OptixApp::edit_perspective_size(unsigned idx, unsigned width, unsigned height) {
    if (idx >= perspectives.size()) return;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, perspectives[idx].gl_buf);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * sizeof(float) * 4, (void*) 0, GL_STREAM_READ);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    perspectives[idx].height = height;
    perspectives[idx].width = width;
    perspectives[idx].image->setFormat(RT_FORMAT_FLOAT4);
    perspectives[idx].image->setSize(width, height);
}


