#pragma once


#include <LittleEngine/geometry.h>

#include <vector>

namespace LittleEngine::Graphics
{

	struct ShadowQuad
	{
		glm::vec2 p1, p2, p3, p4;
	};

	// Light source struct
	struct LightSource
	{
		glm::vec2 position = { 0.f, 0.f };
		glm::vec3 color = { 1.f, 1.f, 1.f };
		float intensity = 1.f;
		float radius = 1.f;


		std::vector<ShadowQuad> GetShadowQuads(const Polygon& poly) const;
	};


	// Returns a vector containing the vertices of the shadow triangles ready for rendering.
	std::vector<glm::vec2> GetShadowTriangles(const std::vector<ShadowQuad>& shadowQuads);



}