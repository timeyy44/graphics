#include <iostream>
#include <vector>

#include "ospray/ospray_cpp.h"

#include "ospray/ospray_cpp/ext/glm.h"
#include "glm/glm.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "tools.h"

using namespace glm;

// helper function to write the rendered image as PPM file
void writePPM(const char* fileName, const ivec2& size, const uint32_t* pixel)
{
	FILE* file;
	errno_t err = fopen_s(&file, fileName, "wb");
	if (err != 0) {
		fprintf(stderr, "fopen('%s', 'wb') failed: %d", fileName, err);
		return;
	}
	fprintf(file, "P6\n%i %i\n255\n", size.x, size.y);
	unsigned char* out = (unsigned char*)alloca(3 * size.x);
	for (int y = 0; y < size.y; y++) {
		const unsigned char* in =
			(const unsigned char*)&pixel[(size.y - 1 - y) * size.x];
		for (int x = 0; x < size.x; x++) {
			out[3 * x + 0] = in[4 * x + 0];
			out[3 * x + 1] = in[4 * x + 1];
			out[3 * x + 2] = in[4 * x + 2];
		}
		fwrite(out, 3 * size.x, sizeof(char), file);
	}
	fprintf(file, "\n");
	fclose(file);
}

ospray::cpp::Material tran_m(Material_s ma) {
	ospray::cpp::Material specular = ospray::cpp::Material("pathtracer", "principled");
	if (ma.m == 0) {
		specular.setParam("baseColor", ma.color);
		specular.setParam("metallic", 0.f);
		//specular.setParam("ior", 3.f);
		//specular.setParam("transmission", 1.f);
	}
	else if (ma.m == 1) {
		specular.setParam("baseColor", ma.color);
		specular.setParam("metallic", ma.ds);
		//specular.setParam("ior", 3.f);
		//specular.setParam("transmission", 1.f);
	}
	else if (ma.m == 2) {
		//specular.setParam("baseColor", fvec3(0.5f, 0.4f, 0.3f));
		//specular.setParam("metallic", 0.f);
		specular.setParam("ior", ma.de);
		specular.setParam("transmission", 1.f);
	}
	specular.commit();
	return specular;
}

int main(int argc, const char** argv)
{
	// image size
	ivec2 imgSize;
	imgSize.x = 1024; // width
	imgSize.y = 768; // height

	// camera
	fvec3 cam_pos{ 0.f, 2.f, -9.f };
	fvec3 cam_up{ 0.f, 1.f, 0.f };
	fvec3 cam_view{ 0.f, 0.f, 1.f };

	// triangle mesh data
	/*std::vector<fvec3> vertex = { fvec3(-1.0f, -.5f, 4.0f),
		fvec3(-1.0f, 1.5f, 4.0f),
		fvec3(1.0f, -.5f, 4.0f),
		fvec3(1.f, 1.5f, 4.f),
		fvec3(-3.f, -3.f, 6.f),
		fvec3(-3.f, 3.f, 6.f),
		fvec3(3.f, -3.f, 6.f),
		fvec3(3.f, 3.f, 6.f),
		//plane
		fvec3(-12.f, -4.f, 0.f),
		fvec3(-12.f, -4.f, 24.f),
		fvec3(12.f, -4.f, 0.f),
		fvec3(12.f, -4.f, 24.f)};

	std::vector<fvec4> color = { fvec4(0.9f, 0.5f, 0.5f, 1.0f),
		fvec4(0.8f, 0.8f, 0.8f, 1.0f),
		fvec4(0.8f, 0.8f, 0.8f, 1.0f),
		fvec4(0.5f, 0.9f, 0.5f, 1.0f),
		fvec4(0.8f, 0.6f, 0.4f, 1.0f),
		fvec4(0.7f, 0.7f, 0.7f, 1.0f),
		fvec4(0.7f, 0.9f, 0.4f, 1.0f),
		fvec4(0.6f, 0.5f, 0.4f, 1.0f),
		//plane
		fvec4(1, 1, 1, 1),
		fvec4(1, 1, 1, 1), 
		fvec4(1, 1, 1, 1), 
		fvec4(1, 1, 1, 1)};

	std::vector<uvec3> index = { uvec3(0, 1, 2), uvec3(1, 2, 3), uvec3(0, 4, 5), uvec3(0, 5, 1), uvec3(0, 4, 6), uvec3(0, 6, 2), uvec3(3, 2, 6), uvec3(3, 6, 7), uvec3(3, 7, 5), uvec3(3, 5, 1), uvec3(8, 9, 11), uvec3(8, 11, 10) };
	*/

	// initialize OSPRay; OSPRay parses (and removes) its commandline parameters,
	// e.g. "--osp:debug"
	OSPError init_error = ospInit(&argc, argv);
	if (init_error != OSP_NO_ERROR)
		return init_error;

	// use scoped lifetimes of wrappers to release everything before ospShutdown()
	{
		// create and setup camera
		ospray::cpp::Camera camera("perspective");
		camera.setParam("aspect", imgSize.x / (float)imgSize.y);
		camera.setParam("position", cam_pos);
		camera.setParam("direction", cam_view);
		camera.setParam("up", cam_up);
		camera.commit(); // commit each object to indicate modifications are done

		// create and setup model and mesh
		/*ospray::cpp::Geometry mesh("mesh");
		mesh.setParam("vertex.position", ospray::cpp::CopiedData(vertex));
		mesh.setParam("vertex.color", ospray::cpp::CopiedData(color));
		mesh.setParam("index", ospray::cpp::CopiedData(index));*/
		//mesh.commit();

		std::vector<fvec3> centers;// = { fvec3(4, 1.5, 4),fvec3(1, 1.5, 4)};
		std::vector<float> radius;// = { 1.5f, 3.f };
		std::vector<Material_s> mess = random_scene_f();
		std::vector<uint32> materials_i;
		std::vector<ospray::cpp::Material> materials;
		int i = 0;
		for (struct Material_s mes : mess) {
			centers.push_back(mes.center);
			radius.push_back(mes.r);
			materials_i.push_back(i++);
			materials.push_back(tran_m(mes));
		}

		ospray::cpp::Geometry sphere("sphere");
		sphere.setParam("sphere.position", ospray::cpp::CopiedData(centers));
		sphere.setParam("sphere.radius", ospray::cpp::CopiedData(radius));
		sphere.commit();

		/*ospray::cpp::Material specular = ospray::cpp::Material("pathtracer", "principled");
		//specular.setParam("ks", fvec3(0.5f, 0.8f, 0.2f));
		//specular.setParam("kd", fvec3(0.3f, 0.6f, 0.2f));
		specular.setParam("baseColor", fvec3(0.5f, 0.4f, 0.3f));
		//specular.setParam("metallic", 0.f);
		specular.setParam("ior", 3.f);
		specular.setParam("transmission", 1.f);
		specular.commit();
		materials.push_back(specular);*/

		// put the mesh into a model
		//ospray::cpp::GeometricModel model(mesh);
		//model.commit();

		ospray::cpp::GeometricModel model_2(sphere);
		model_2.setParam("material", ospray::cpp::CopiedData(materials_i));
		model_2.commit();

		// put the model into a group (collection of models)
		ospray::cpp::Group group;
		group.setParam("geometry", ospray::cpp::CopiedData(model_2));
		group.commit();

		// put the group into an instance (give the group a world transform)
		ospray::cpp::Instance instance(group);
		instance.commit();

		// put the instance in the world
		ospray::cpp::World world;
		world.setParam("instance", ospray::cpp::CopiedData(instance));

		// create and setup light for Ambient Occlusion
		ospray::cpp::Light light("sunSky");
		light.commit();

		ospray::cpp::Light light_spot("spot");
		light_spot.setParam("color", fvec3(0.2, 0.4, 0.6));
		light_spot.setParam("intensity", 30.f);
		light_spot.setParam("position", fvec3(0, 0, 0));
		light_spot.setParam("direction", fvec3(0, 0, 1));
		light_spot.setParam("openingAngle", 45.f);
		light_spot.setParam("penumbraAngle", 90.f);
		light_spot.commit();

		std::vector<ospray::cpp::Light> lights = { light, light_spot };

		world.setParam("light", ospray::cpp::CopiedData(lights));
		world.commit();

		// create renderer, choose Scientific Visualization renderer
		ospray::cpp::Renderer renderer("pathtracer");

		// complete setup of renderer
		renderer.setParam("roulettePathLength", 10);
		renderer.setParam("backgroundColor", fvec3(1.f)); // white, transparent
		renderer.setParam("pixelSamples", 10);
		//renderer.setParam("backgroundRefraction", true);
		renderer.setParam("material", ospray::cpp::CopiedData(materials));
		renderer.commit();

		// create and setup framebuffer
		ospray::cpp::FrameBuffer framebuffer(
			imgSize.x, imgSize.y, OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);
		framebuffer.clear();

		// render one frame
		framebuffer.renderFrame(renderer, camera, world);

		// access framebuffer and write its content as PPM file
		uint32_t* fb = (uint32_t*)framebuffer.map(OSP_FB_COLOR);
		//writePPM("firstFrameCpp.ppm", imgSize, fb);
		stbi_flip_vertically_on_write(true);
		stbi_write_png("./test.png", imgSize.x, imgSize.y, 4, (const unsigned char*)fb, 0);
		framebuffer.unmap(fb);

		renderer.setParam("pixelSamples", 1);
		renderer.commit();

		// render 10 more frames, which are accumulated to result in a better
		// converged image
		for (int frames = 0; frames < 10; frames++) {
			framebuffer.renderFrame(renderer, camera, world);
			std::cout << frames << std::endl;
		}

		fb = (uint32_t*)framebuffer.map(OSP_FB_COLOR);
		//writePPM("accumulatedFrameCpp.ppm", imgSize, fb);
		stbi_write_png("./test_2.png", imgSize.x, imgSize.y, 4, (const unsigned char*)fb, 0);
		framebuffer.unmap(fb);

		ospray::cpp::PickResult res =
			framebuffer.pick(renderer, camera, world, 0.5f, 0.5f);

		if (res.hasHit) {
			std::cout << "Picked geometry [inst: " << res.instance.handle()
				<< ", model: " << res.model.handle() << ", prim: " << res.primID
				<< "]" << std::endl;
		}
	}

	ospShutdown();

	return 0;
}
