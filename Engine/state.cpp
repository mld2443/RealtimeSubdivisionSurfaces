//
//  state.cpp
//  adapted from Spring
//
//  Created by Matthew Dillard on 11/4/15.
//

#include "state.h"

state::vector::vector() {
	m_dragc = 0.0f;
	m_liftc = 0.0f;
	m_vertices = 0;
	m_cols = 0;
	m_rows = 0;
	m_vertexCount = 0;
	m_indexCount = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

state::vector::vector(const vector& s): m_edges(s.m_edges), m_faces(s.m_faces) {
	m_particles.resize(s.size());

	for (unsigned long i = 0; i < s.size(); ++i)
		m_particles[i] = s[i];
}

state::vector::~vector() {}


bool state::vector::Initialize(ID3D11Device* device, const v3 x1, const v3 x2, unsigned int cols, unsigned int rows, float dragc, float liftc, bool sideways)
{
	bool result;


	// add drag and lift coefficients
	m_dragc = dragc;
	m_liftc = liftc;
	m_cols = cols;
	m_rows = rows;

	// Initialize the particle system.
	result = InitializeParticleSystem(x1, x2, sideways);
	if (!result)
	{
		return false;
	}

	// Create the buffers that will be used to render the particles with.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void state::vector::Shutdown()
{
	// Release the buffers.
	ShutdownBuffers();

	// Release the particle system.
	ShutdownParticleSystem();

	return;
}

bool state::vector::Frame(float frameTime, ID3D11DeviceContext* deviceContext)
{
	bool result;


	// Update the position of the particles.
	UpdateParticles(frameTime * 0.001f);

	// Update the dynamic vertex buffer with the new position of each particle.
	result = UpdateBuffers(deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void state::vector::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


state::vector state::vector::derive(const float t) {
	vector Sdot;
	Sdot.m_faces = m_faces;
	Sdot.m_edges = m_edges;
	Sdot.m_particles.resize(size());

	for (unsigned long i = 0; i < size(); ++i) {
		if (m_particles[i]) {
			Sdot[i] = { m_particles[i].vel,{0,0,0}, m_particles[i].red, m_particles[i].green, m_particles[i].blue, m_particles[i].mass, m_particles[i].fixed };
		}
		else {
			v3 accel;

			accel += gravity * t;

			Sdot[i] = {m_particles[i].vel,accel, m_particles[i].red, m_particles[i].green, m_particles[i].blue, m_particles[i].mass, m_particles[i].fixed};
		}
	}

	/*for (auto &col : *m_faces) {
		for (auto &f : col) {
			v3 P0(m_particles[f.i0].pos), P1(m_particles[f.i1].pos), P2(m_particles[f.i2].pos), P3(m_particles[f.i3].pos);
			v3 V0(m_particles[f.i0].vel), V1(m_particles[f.i1].vel), V2(m_particles[f.i2].vel), V3(m_particles[f.i3].vel);
			v3 normal((P2 - P0).cross(P3 - P1));

			v3 Vf((V0 + V1 + V2 + V3) / 4);
			v3 Vr(Vf - (wind * ((float)rand() / RAND_MAX)));
			float A = normal.abs();
			normal /= A;

			v3 Fd{ 0.0,0.0,0.0 }, Fl{ 0.0,0.0,0.0 };
			if (Vr.x != 0.0 && Vr.y != 0.0 && Vr.z != 0.0) {
				Fd = Vr.normalize() * -m_dragc * A*(normal.dot(Vr));
				v3 ncv(normal.cross(Vr));
				if (ncv.x != 0.0 && ncv.y != 0.0 && ncv.z != 0.0)
					Fl = (Vr.cross(ncv.normalize())) * -m_liftc * A*(normal.dot(Vr));
			}

			float R0 = (P1 - P0).angle(P3 - P0) / 360.0f;
			float R1 = (P2 - P1).angle(P0 - P1) / 360.0f;
			float R2 = (P3 - P2).angle(P1 - P2) / 360.0f;
			float R3 = (P0 - P3).angle(P2 - P3) / 360.0f;

			if (!m_particles[f.i0]) Sdot[f.i0].vel += ((Fd + Fl) * R0) / Sdot[f.i0].mass;
			if (!m_particles[f.i1]) Sdot[f.i1].vel += ((Fd + Fl) * R1) / Sdot[f.i1].mass;
			if (!m_particles[f.i2]) Sdot[f.i2].vel += ((Fd + Fl) * R2) / Sdot[f.i2].mass;
			if (!m_particles[f.i3]) Sdot[f.i3].vel += ((Fd + Fl) * R3) / Sdot[f.i3].mass;
		}
	}*/

	for (auto &e : *m_edges) {
		const mesh::vertex *Vi = &m_particles[e.i1], *Vj = &m_particles[e.i2];

		float Lij = (Vj->pos - Vi->pos).abs();
		v3 Xij = (Vj->pos - Vi->pos).normalize();

		v3 force = v3();
		force += Xij * e.k * (Lij - e.l);
		force += Xij * e.d * (Vj->vel - Vi->vel).dot(Xij);

		if (!Sdot[e.i1]) {
			Sdot[e.i1].vel += (force / Vi->mass);
			if (Sdot[e.i2])
				Sdot[e.i1].vel += (force / Vi->mass);
		}
		if (!Sdot[e.i2]) {
			Sdot[e.i2].vel -= (force / Vj->mass);
			if (Sdot[e.i1])
				Sdot[e.i2].vel -= (force / Vj->mass);
		}
	}

	return Sdot;
}

void state::vector::handleCollisions(vector& Snew) {
	for (unsigned int i = 0; i < size(); ++i) {
		for (auto &col : *m_faces) {
			for (auto &f : col) {
				const v3 P1(Snew[f.i0].pos), P2(Snew[f.i1].pos), P3(Snew[f.i2].pos), P4(Snew[f.i3].pos);
				const v3 normal(((P3 - P1).cross(P4 - P2)).normalize());
				if (f.i0 != i && f.i1 != i && f.i2 != i && f.i3 != i) {
					if (f.cross_plane(P1, normal, m_particles[i].pos, Snew[i].pos)) {
						v3 intersect = f.find_intersect(P1, normal, m_particles[i].pos, Snew[i].pos);
						if (f.projection_intersection(normal, P1, P2, P3, P4, intersect)) {
							v3 d = normal * f.dist_from_plane(P1, normal, Snew[i].pos);
							Snew[i].pos = Snew[i].pos - (d * 2);

							v3 v_norm = normal * (Snew[i].vel.dot(normal));
							v3 v_tan = Snew[i].vel - v_norm;
							Snew[i].vel = (v_norm * -1) + v_tan;
						}
					}
				}
			}
		}
	}
}


state::vector state::vector::operator+(const vector& s) const {
	vector rvalue;
	rvalue.m_faces = m_faces;
	rvalue.m_edges = m_edges;
	rvalue.m_particles.resize(size());

	for (unsigned long i = 0; i < size(); ++i)
		rvalue[i] = m_particles[i] + s.m_particles[i];

	return rvalue;
}

state::vector state::vector::operator-(const vector& s) const {
	vector rvalue;
	rvalue.m_faces = m_faces;
	rvalue.m_edges = m_edges;
	rvalue.m_particles.resize(size());

	for (unsigned long i = 0; i < size(); ++i)
		rvalue[i] = m_particles[i] - s.m_particles[i];

	return rvalue;
}

state::vector state::vector::operator*(const float value) const {
	vector rvalue;
	rvalue.m_faces = m_faces;
	rvalue.m_edges = m_edges;
	rvalue.m_particles.resize(size());

	for (unsigned long i = 0; i < size(); ++i)
		rvalue[i] = m_particles[i] * value;

	return rvalue;
}

state::vector& state::vector::operator=(const vector& s) {
	m_particles.clear();
	m_faces = s.m_faces;
	m_edges = s.m_edges;
	m_particles.resize(s.size());

	for (unsigned long i = 0; i < s.size(); ++i)
		m_particles[i] = s[i];
	return *this;
}


unsigned long state::vector::index(const unsigned int x, const unsigned int y) { return x * (m_rows + 1) + y; }

void state::vector::addEdgesandFaces(const unsigned int x, const unsigned int y, const float dist) {
	if (y) {
		m_edges->push_back({ 0.5f, 0.5f, dist, index(x, y), index(x, y - 1) });
		if (x) {
			m_edges->push_back({ 0.5f, 0.5f, dist, index(x, y), index(x - 1, y) });
			m_faces->back().push_back({ index(x - 1, y - 1), index(x - 1, y), index(x, y), index(x, y - 1) });
		}
	}
	if (x) {
		if (y)
			m_edges->push_back({ 0.5f, 0.5f, SQRT2 * dist, index(x, y), index(x - 1, y - 1) });
		if (y < m_rows)
			m_edges->push_back({ 0.5f, 0.5f, SQRT2 * dist, index(x, y), index(x - 1, y + 1) });
	}
}


bool state::vector::InitializeParticleSystem(const v3 x1, const v3 x2, bool sideways)
{
	m_edges = new std::vector<mesh::strut>;
	if (!m_edges)
	{
		return false;
	}

	m_faces = new std::vector<std::vector<mesh::face>>;
	if (!m_faces)
	{
		return false;
	}

	const v3 step((x2 - x1) / (float)m_cols);
	float dist = step.abs();
	v3 down((sideways)? step.cross({0.0f,1.0f,0.0f}).normalize() * dist : step.cross({0.0f,-1.0f,0.0f}).cross(step).normalize() * dist);
	v3 top(x1), lower;
	for (unsigned int i = 0; i <= m_cols; ++i) {
		m_particles.push_back({top, {0.0f,0.0f,0.0f}, 0.0f, 1.0f, 1.0f, 1.0f, true});
		if (i) m_faces->push_back(std::vector<mesh::face>());

		addEdgesandFaces(i, 0, dist);
		lower = top + down;
		for (unsigned int j = 1; j <= m_rows; ++j) {
			m_particles.push_back({ lower, {0.0f,0.0f,0.0f}, 0.0f, 1.0f, 1.0f, 0.1f, false });
			addEdgesandFaces(i, j, dist);
			lower += down;
		}
		top += step;
	}

	return true;
}

void state::vector::ShutdownParticleSystem()
{
	// Release the particle list.
	m_particles.clear();
	m_cols = 0;
	m_rows = 0;

	// Release the vertex list.
	if (m_vertices)
	{
		delete[] m_vertices;
		m_vertices = 0;
		m_vertexCount = 0;
	}

	// Release the edge list.
	if (m_edges)
	{
		m_edges->clear();
		delete m_edges;
		m_edges = 0;
	}

	// Release the face list.
	if (m_faces)
	{
		m_faces->clear();
		delete m_faces;
		m_faces = 0;
	}

	return;
}


bool state::vector::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	unsigned int i = 0;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the maximum number of vertices in the vertex array.
	m_vertexCount = m_particles.size();

	// Set the maximum number of indices in the index array.
	m_indexCount = (m_faces->size() - 2) * (m_faces->back().size() - 2) * 16;

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Initialize the index array.
	for (unsigned int y = 1; y < m_rows - 1; ++y)
	{
		for (unsigned int x = 1; x < m_cols - 1; ++x)
		{
			indices[i] = index(x - 1, y - 1);
			indices[i+1] = index(x, y - 1);
			indices[i+2] = index(x + 1, y - 1);
			indices[i+3] = index(x + 2, y - 1);
			indices[i+4] = index(x - 1, y);
			indices[i+5] = index(x, y);
			indices[i+6] = index(x + 1, y);
			indices[i+7] = index(x + 2, y);
			indices[i+8] = index(x - 1, y + 1);
			indices[i+9] = index(x, y + 1);
			indices[i+10] = index(x + 1, y + 1);
			indices[i+11] = index(x + 2, y + 1);
			indices[i+12] = index(x - 1, y + 2);
			indices[i+13] = index(x, y + 2);
			indices[i+14] = index(x + 1, y + 2);
			indices[i+15] = index(x + 2, y + 2);
			i += 16;
		}
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}

void state::vector::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void state::vector::UpdateParticles(float frameTime)
{
	auto Snew = RK4(*this, frameTime);

	//handleCollisions(Snew);

	m_particles = Snew.m_particles;

	return;
}


bool state::vector::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;


	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Now build the vertex array from the particle list array.
	for (unsigned int i = 0; i < m_vertexCount; ++i)
	{
		m_vertices[i].position = XMFLOAT3(m_particles[i].pos.x, m_particles[i].pos.y, m_particles[i].pos.z);
		m_vertices[i].color = XMFLOAT4(m_particles[i].red, m_particles[i].green, m_particles[i].blue, 1.0f);
	}

	// Lock the vertex buffer.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}

void state::vector::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST);

	return;
}


v3 state::gravity;
v3 state::wind;

state::vector state::RK1(vector S, const float t) {
	return S + S.derive(t) * t;
}

state::vector state::RK2(vector S, const float t) {
	auto K1(S.derive(t));
	auto K2((S + (K1 * (t / 2.0f))).derive(t / 2.0f));
	return S + (K2 * t);
}

state::vector state::RK4(vector S, const float t) {
	auto K1(S.derive(t));
	auto K2((S + (K1 * (t / 2.0f))).derive(t / 2.0f));
	auto K3((S + (K2 * (t / 2.0f))).derive(t / 2.0f));
	auto K4((S + (K3 * t)).derive(t));
	return S + ((K1 + (K2 * 2.0f) + (K3 * 2.0f) + K4) * (t / 6.0f));
}