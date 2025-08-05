#pragma once

#include <vector>
#include <glm/glm.hpp>


namespace LittleEngine::Math
{

	// Edge struct, representing a line segment between two points
	// oriented from p1 to p2 
	// with normal pointing to the right (outward in CCW polygons)

	struct Edge
	{
		glm::vec2 p1;
		glm::vec2 p2;

		float length() const { return glm::length(p2 - p1); }
		glm::vec2 direction() const { return glm::normalize(p2 - p1); }
		glm::vec2 normal() const;

	};

	/** Returns the orientation of the triplet (a, b, c).
	 * 0 -> collinear
	 * 1 -> clockwise
	 * 2 -> counterclockwise
	 */
	int ThreePointOrientation(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

	float TriangleSignedArea(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

	bool SegmentsIntersect(const Edge& e1, const Edge& e2);
	bool PointOnSegment(const glm::vec2& p, const Edge& e);

	// Polygon struct, representing a polygon with vertices and edges
	// Edges are calculated from vertices, so they should be recalculated after modifying vertices
	// IMPORTANT : Vertices are in CCW order !!
	struct Polygon
	{
		std::vector<glm::vec2> vertices;

		bool IsValid() const;
		bool IsCounterClockwise() const;
		void EnsureCounterClockwise();
		float SignedArea() const;
		bool IsSelfIntersecting() const;

		std::vector<Edge> GetEdges() const;
	};

} // namespace LittleEngine