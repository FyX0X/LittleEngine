#include "LittleEngine/Math/geometry.h"
#include "LittleEngine/Utils/logger.h"

namespace LittleEngine::Math
{

#pragma region Edge

	glm::vec2 Edge::normal() const
	{
		glm::vec2 dir = direction();
		return glm::vec2(dir.y, -dir.x); // right normal in CCW order is outside
	}

	int ThreePointOrientation(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
	{
		float signedDoubleArea = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
		if (signedDoubleArea == 0)
			return 0; // collinear
		else if (signedDoubleArea < 0)
			return 1; // clockwise
		else
			return 2; // counterclockwise
	}

	float TriangleSignedArea(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
	{
		return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)) * 0.5f;
	}

	bool SegmentsIntersect(const Edge& e1, const Edge& e2)
	{
		int o1 = ThreePointOrientation(e1.p1, e1.p2, e2.p1);
		int o2 = ThreePointOrientation(e1.p1, e1.p2, e2.p2);
		int o3 = ThreePointOrientation(e2.p1, e2.p2, e1.p1);
		int o4 = ThreePointOrientation(e2.p1, e2.p2, e1.p2);
		
		// general case
		if (o1 != o2 && o3 != o4)
			return true;

		// special cases

		// e1.p1, e1.p2 and e2.p1 are collinear and e2.p1 lies on segment e1
		if (o1 == 0 && PointOnSegment(e2.p1, e1))
			return true;

		// e1.p1, e1.p2 and e2.p2 are collinear and e2.p2 lies on segment e1
		if (o2 == 0 && PointOnSegment(e2.p2, e1))
			return true;

		// e2.p1, e2.p2 and e1.p1 are collinear and e1.p1 lies on segment e2
		if (o3 == 0 && PointOnSegment(e1.p1, e2))
			return true;

		// e2.p1, e2.p2 and e1.p2 are collinear and e1.p2 lies on segment e2
		if (o4 == 0 && PointOnSegment(e1.p2, e2))
			return true;

		return false; // doesn't intersect
	}

	bool PointOnSegment(const glm::vec2& p, const Edge& e)
	{
		return (ThreePointOrientation(e.p1, e.p2, p) == 0) &&	// colinear
			p.x >= std::min(e.p1.x, e.p2.x) &&
			p.x <= std::max(e.p1.x, e.p2.x)	&&
			p.y >= std::min(e.p1.y, e.p2.y)	&&
			p.y <= std::max(e.p1.y, e.p2.y);
	}



#pragma endregion


	bool Polygon::IsValid() const
	{
		return (IsCounterClockwise() && !IsSelfIntersecting() && vertices.size() >= 3);
	}

	bool Polygon::IsCounterClockwise() const
	{
		return SignedArea() < 0.f; // negative area indicates counter-clockwise orientation
	}

	void Polygon::EnsureCounterClockwise()
	{
		if (!IsCounterClockwise())
		{
			std::reverse(vertices.begin(), vertices.end());
			Utils::Logger::Info("Polygon::EnsureClockwise: reversing vertices.");
		}
	}

	float Polygon::SignedArea() const
	{
		float area = 0.f;
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			const glm::vec2& a = vertices[i];
			const glm::vec2& b = vertices[(i + 1) % vertices.size()];
			area += (b.x - a.x) * (b.y + a.y);
		}
		return area * 0.5f;
	}

	bool Polygon::IsSelfIntersecting() const
	{
		std::vector<Edge> edges = GetEdges();
		size_t n = edges.size();
		for (size_t i = 0; i < n; ++i)
		{
			for (size_t j = i + 1; j < n; ++j)
			{
				if (j == (i + 1) % n || j == (i + n - 1) % n)
					continue; // skip adjacent edges

				if (SegmentsIntersect(edges[i], edges[j]))
					return true;
			}
		}
		return false;
	}


	std::vector<Edge> Polygon::GetEdges() const
	{
		std::vector<Edge> edges;
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			glm::vec2 p1 = vertices[i];
			glm::vec2 p2 = vertices[(i + 1) % vertices.size()];
			edges.push_back({ p1, p2 });
		}
		return edges;
	}

} // namespace LittleEngine