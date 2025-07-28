#include "LittleEngine/Graphics/lighting.h"


namespace LittleEngine::Graphics
{
	std::vector<ShadowQuad> LightSource::GetShadowQuads(const Polygon& poly) const
	{
		std::vector<ShadowQuad> shadowQuads;
		for (const Edge& edge : poly.GetEdges())
		{
			if (ThreePointOrientation(edge.p1, edge.p2, position) == 1)	// if clockwise, position in on the right side of the edge (faces light)
			{
				glm::vec2 dir1 = glm::normalize(edge.p1 - position);
				glm::vec2 dir2 = glm::normalize(edge.p2 - position);
				// Create shadow quad vertices
				glm::vec2 p1 = edge.p1;
				glm::vec2 p2 = edge.p2;
				glm::vec2 p3 = edge.p2 + dir2 * radius * 100.f;
				glm::vec2 p4 = edge.p1 + dir1 * radius * 100.f;
				shadowQuads.push_back({ p1, p2, p3, p4 });
			}
		}

		return shadowQuads;
	}

	std::vector<glm::vec2> GetShadowTriangles(const std::vector<ShadowQuad>& shadowQuads)
	{
		std::vector<glm::vec2> triangles;
		for (const ShadowQuad& quad : shadowQuads)
		{
			// Create two triangles from the quad
			triangles.push_back(quad.p1);
			triangles.push_back(quad.p2);
			triangles.push_back(quad.p3);
			triangles.push_back(quad.p1);
			triangles.push_back(quad.p3);
			triangles.push_back(quad.p4);
		}
		return triangles;
	}
}