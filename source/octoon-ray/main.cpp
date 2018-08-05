#include <assert.h>
#include <fstream>
#include <random>
#include <GLFW/glfw3.h>
#include <GL/GL.h>

#include "MonteCarloThread.h"

void dumpTGA(std::ostream& stream, std::uint8_t pixesl[], std::uint32_t width, std::uint32_t height, std::uint8_t channel) noexcept
{
	assert(stream);
	assert(width < std::numeric_limits<std::uint16_t>::max() || height < std::numeric_limits<std::uint16_t>::max());

	std::uint8_t  id_length = 0;
	std::uint8_t  colormap_type = 0;
	std::uint8_t  image_type = 2;
	std::uint16_t colormap_index = 0;
	std::uint16_t colormap_length = 0;
	std::uint8_t  colormap_size = 0;
	std::uint16_t x_origin = 0;
	std::uint16_t y_origin = 0;
	std::uint16_t w = (std::uint16_t)width;
	std::uint16_t h = (std::uint16_t)height;
	std::uint8_t  pixel_size = channel * 8;
	std::uint8_t  attributes = channel == 4 ? 8 : 0;

	stream.write((char*)&id_length, sizeof(id_length));
	stream.write((char*)&colormap_type, sizeof(colormap_type));
	stream.write((char*)&image_type, sizeof(image_type));
	stream.write((char*)&colormap_index, sizeof(colormap_index));
	stream.write((char*)&colormap_length, sizeof(colormap_length));
	stream.write((char*)&colormap_size, sizeof(colormap_size));
	stream.write((char*)&x_origin, sizeof(x_origin));
	stream.write((char*)&y_origin, sizeof(y_origin));
	stream.write((char*)&w, sizeof(w));
	stream.write((char*)&h, sizeof(h));
	stream.write((char*)&pixel_size, sizeof(pixel_size));
	stream.write((char*)&attributes, sizeof(attributes));

	std::vector<std::uint8_t> buffer(width * height * channel);
	for (std::size_t i = 0; i < buffer.size(); i++)
		buffer[i] = pixesl[i];

	stream.write((char*)buffer.data(), width * height * channel);
}

void dumpTGA(const char* filepath, std::uint8_t pixesl[], std::uint32_t width, std::uint32_t height, std::uint8_t channel) noexcept(false)
{
	auto stream = std::ofstream(filepath, std::ios_base::out | std::ios_base::binary);
	if (stream)
		dumpTGA(stream, pixesl, width, height, channel);
	else
		throw std::runtime_error("failed to open the file: " + std::string(filepath));
}

int main(int argc, const char* argv[])
{
	auto width = 1920;
	auto height = 1080;

	if (::glfwInit() == GL_FALSE)
		return 0;

	auto window = ::glfwCreateWindow(width, height, "Octoon Ray", nullptr, nullptr);
	if (window)
	{
		::glfwMakeContextCurrent(window);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLuint texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, GL_NONE);

		glEnable(GL_TEXTURE_2D);

		octoon::MonteCarloThread engine(width, height);

		for (std::uint32_t frame = 1; ; frame++)
		{
			std::uint16_t tileSize = 64;
			std::uint16_t tileNumsX = width / tileSize + (width % tileSize > 0 ? 1 : 0);
			std::uint16_t tileNumsY = height / tileSize + (height % tileSize > 0 ? 1 : 0);

			std::vector<int> tiles(tileNumsX * tileNumsY);
			std::vector<std::future<std::uint32_t>> queues;

			for (std::int32_t i = 0; i < tileNumsX * tileNumsY; i++)
				tiles[i] = i;

			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(tiles.begin(), tiles.end(), g);

			for (auto& it : tiles)
				queues.push_back(engine.render(frame, it));

			for (auto& it : queues)
			{
				if (::glfwWindowShouldClose(window))
					goto exit;

				it.get();
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, engine.data());

				int w = 0, h = 0;
				glfwGetWindowSize(window, &w, & h);
				glViewport(0, 0, w, h);

				glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
				glEnd();

				glfwPollEvents();
				glfwSwapBuffers(window);
			}
		}
	}

exit:
    return 0;
}