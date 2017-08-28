//
//  state.h
//  Spring
//
//  Created by Matthew Dillard on 11/2/15.
//  
//

#ifndef state_h
#define state_h

#include <d3d11.h>
#include <DirectXmath.h>
#include <vector>
#include <list>

#include "mesh.h"

#define SQRT2 1.414213538f

using namespace DirectX;

namespace state {
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	class vector {
	private:
		std::vector<mesh::vertex> m_particles;
		std::vector<std::vector<mesh::face>> *m_faces;
		std::vector<mesh::strut> *m_edges;

		float m_dragc, m_liftc;

		VertexType *m_vertices;

		ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
		unsigned int m_cols, m_rows, m_vertexCount, m_indexCount;

	public:
		vector();
		vector(const vector& s);
		~vector();

		bool Initialize(ID3D11Device*, const v3, const v3, unsigned int, unsigned int, float, float, bool sideways = false);
		void Shutdown();
		bool Frame(float, ID3D11DeviceContext*);
		void Render(ID3D11DeviceContext*);

		std::vector<mesh::vertex>::const_iterator begin() const { return m_particles.begin(); }
		std::vector<mesh::vertex>::const_iterator end() const { return m_particles.end(); }
		size_t size() const { return m_particles.size(); }

		void add_vertex(const mesh::vertex& v) { m_particles.push_back(v); }

		vector derive(const float t);
		void handleCollisions(vector& Snew);

		mesh::vertex& operator[](unsigned long i) { return m_particles[i]; }
		const mesh::vertex& operator[](unsigned long i) const { return m_particles[i]; }
		unsigned int GetIndexCount() const { return m_indexCount; }

		vector operator+(const vector& s) const;
		vector operator-(const vector& s) const;
		vector operator*(const float value) const;
		vector& operator=(const vector& s);

	private:
		unsigned long index(const unsigned int, const unsigned int);
		void addEdgesandFaces(const unsigned int, const unsigned int, const float);

		bool InitializeParticleSystem(const v3, const v3, bool);
		void ShutdownParticleSystem();

		bool InitializeBuffers(ID3D11Device*);
		void ShutdownBuffers();


		void UpdateParticles(float);

		bool UpdateBuffers(ID3D11DeviceContext*);
		void RenderBuffers(ID3D11DeviceContext*);
	};

	extern v3 gravity;
	extern v3 wind;

	vector RK1(vector, const float);
	vector RK2(vector, const float);
	vector RK4(vector, const float);
}

#endif /* state_h */
