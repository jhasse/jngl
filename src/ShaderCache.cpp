// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "ShaderCache.hpp"

#include "jngl/Shader.hpp"
#include "opengl.hpp"
#include "spriteimpl.hpp"

#include <cassert>

namespace jngl {

ShaderCache::ShaderCache() {
	Shader vertexShader(R"(#version 300 es
		in mediump vec2 position;
		uniform highp mat3 modelview;
		uniform mediump mat4 projection;

		void main() {
			vec3 tmp = modelview * vec3(position, 1);
			gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
		})",
	                    Shader::Type::VERTEX, R"(#version 100
		attribute mediump vec2 position;
		uniform highp mat3 modelview;
		uniform mediump mat4 projection;

		void main() {
			vec3 tmp = modelview * vec3(position, 1);
			gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
		})");
	Shader fragmentShader(R"(#version 300 es
		uniform lowp vec4 color;
		out lowp vec4 outColor;

		void main() {
			outColor = color;
		})",
	                      Shader::Type::FRAGMENT, R"(#version 100
		uniform lowp vec4 color;

		void main() {
			gl_FragColor = color;
		})");
	simpleShaderProgram = std::make_unique<ShaderProgram>(vertexShader, fragmentShader);
	simpleModelviewUniform = simpleShaderProgram->getUniformLocation("modelview");
	simpleColorUniform = simpleShaderProgram->getUniformLocation("color");

	{
		textureVertexShader = std::make_unique<Shader>(R"(#version 300 es
			in mediump vec2 position;
			in mediump vec2 inTexCoord;
			uniform highp mat3 modelview;
			uniform mediump mat4 projection;
			out mediump vec2 texCoord;

			void main() {
				vec3 tmp = modelview * vec3(position, 1);
				gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
				texCoord = inTexCoord;
			})",
		                                               Shader::Type::VERTEX, R"(#version 100
			attribute mediump vec2 position;
			attribute mediump vec2 inTexCoord;
			uniform highp mat3 modelview;
			uniform mediump mat4 projection;
			varying mediump vec2 texCoord;

			void main() {
				vec3 tmp = modelview * vec3(position, 1);
				gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
				texCoord = inTexCoord;
			})");
		Shader fragmentShader(R"(#version 300 es
			uniform sampler2D tex;
			uniform lowp vec4 spriteColor;

			in mediump vec2 texCoord;

			out lowp vec4 outColor;

			void main() {
				outColor = texture(tex, texCoord) * spriteColor;
			})",
		                      Shader::Type::FRAGMENT, R"(#version 100
			uniform sampler2D tex;
			uniform lowp vec4 spriteColor;

			varying mediump vec2 texCoord;

			void main() {
				gl_FragColor = texture2D(tex, texCoord) * spriteColor;
			})");
		textureShaderProgram =
		    std::make_unique<ShaderProgram>(*textureVertexShader, fragmentShader);
		shaderSpriteColorUniform = textureShaderProgram->getUniformLocation("spriteColor");
		modelviewUniform = textureShaderProgram->getUniformLocation("modelview");
	}
}

ShaderCache::~ShaderCache() = default;

ShaderProgram::Context ShaderCache::useSimpleShaderProgram() {
	return useSimpleShaderProgram(opengl::modelview, gShapeColor);
}

ShaderProgram::Context ShaderCache::useSimpleShaderProgram(const Mat3& modelview, Rgba color) {
	auto context = simpleShaderProgram->use();
	glUniform4f(simpleColorUniform, color.getRed(), color.getGreen(), color.getBlue(),
	            color.getAlpha());
	glUniformMatrix3fv(simpleModelviewUniform, 1, GL_FALSE, modelview.data);

	assert(simpleShaderProgram->getAttribLocation("position") == 0);
	glEnableVertexAttribArray(0);

	return context;
}

} // namespace jngl
