#include "RenderMesh.h"

#include "ModuleMesh.h"
#include "ResourceMesh.h"
#include "ModuleMaterial.h"
#include "Glew/include/glew.h"
#include "Color.h"

RenderMesh::RenderMesh(ModuleMesh* mesh, ModuleMaterial* material, float4x4 gTransform, Color color) :
	mesh(mesh), material(material), transform(gTransform),color(color)
{
	if (material != nullptr)
	{
		this->color = material->matCol;
	}
}

RenderMesh::~RenderMesh()
{
	mesh = nullptr;
	material = nullptr;
}

void RenderMesh::Draw(MeshDrawMode sceneMaxDrawMode)
{
	float4x4 newTrans = transform;
	newTrans.Transpose();

	ResourceMesh* m = mesh->GetTemporalMesh();

	if (m == nullptr)
	{
		m = mesh->GetMesh();
	}


	glEnableClientState(GL_VERTEX_ARRAY);	//... TODO (1) Put this on start of render postupdate
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glPushMatrix();
	glMultMatrixf(newTrans.v[0]);


	if ((mesh->normalDrawMode == (int)NormalDrawMode::NORMAL_MODE_VERTEX || mesh->normalDrawMode == (int)NormalDrawMode::NORMAL_MODE_BOTH) && !m->normals.empty())
		DrawVertexNormals(m);

	if ((mesh->normalDrawMode == (int)NormalDrawMode::NORMAL_MODE_FACES || mesh->normalDrawMode == (int)NormalDrawMode::NORMAL_MODE_BOTH) && !m->normals.empty())
		DrawFacesNormals(m);

	glColor4f(color.r, color.g, color.b, color.a);

	int localDrawMode = (int)sceneMaxDrawMode;

	if (mesh->meshDrawMode > localDrawMode)
		localDrawMode = mesh->meshDrawMode;


	if (localDrawMode == (int)MeshDrawMode::DRAW_MODE_WIRE)
	{
		glColor4f(0.5f,0.5f,0.5f,0.5f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (localDrawMode == (int)MeshDrawMode::DRAW_MODE_FILL)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glColor4f(0.5f, 0.5f, 0.5f, 0.5f);

		DrawBuffers(m);

		glColor4f(color.r, color.g, color.b, color.a);


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}

	DrawBuffers(m);


	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY); // ... TODO (2) Put this on end of render postupdate
	glDisableClientState(GL_NORMAL_ARRAY); // ... TODO (2) Put this on end of render postupdate
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//clear buffers

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void RenderMesh::DrawVertexNormals(ResourceMesh* m)
{
	float magnitude = mesh->normalVertexSize;
	glColor4f(1.0f, 0.5f, 0.0f,1.0f);
	glLineWidth(4.0f);
	glBegin(GL_LINES);


	for (int i = 0; i < mesh->GetMesh()->normals.size() / 3; i++)
	{
		float3 vertex0 = { m->vertices[i * 3], m->vertices[(i * 3) + 1], m->vertices[(i * 3) + 2] };
		float3 vertex1 =
		{
			m->vertices[i * 3] + (m->smoothedNormals[i * 3] * magnitude),
			m->vertices[(i * 3) + 1] + (m->smoothedNormals[(i * 3) + 1] * magnitude),
			m->vertices[(i * 3) + 2] + (m->smoothedNormals[(i * 3) + 2] * magnitude)
		};


		glVertex3f(vertex0.x, vertex0.y, vertex0.z);
		glVertex3f(vertex1.x, vertex1.y, vertex1.z);
	}



	glEnd();

	glLineWidth(2.0f);
}

void RenderMesh::DrawFacesNormals(ResourceMesh* m)
{

	float magnitude = mesh->normalFaceSize;
	glColor4f(0.0f, 0.25f, 1.0f,1.0f);
	glLineWidth(4.0f);


	glBegin(GL_LINES);

	for (int i = 0; i < m->indices.size(); i += 3)
	{
		unsigned int vertex0id = m->indices[i];
		unsigned int vertex1id = m->indices[i + 1];
		unsigned int vertex2id = m->indices[i + 2];


		float3 vertex0 = { m->vertices[vertex0id * 3],m->vertices[(vertex0id * 3) + 1], m->vertices[(vertex0id * 3) + 2] };
		float3 vertex1 = { m->vertices[vertex1id * 3], m->vertices[(vertex1id * 3) + 1], m->vertices[(vertex1id * 3) + 2] };
		float3 vertex2 = { m->vertices[vertex2id * 3], m->vertices[(vertex2id * 3) + 1], m->vertices[(vertex2id * 3) + 2] };

		float3 vector01 = vertex1 - vertex0;//vector from point 0 to point 1
		float3 vector02 = vertex2 - vertex0;//vector from point 0 to point 2
		float3 normal = Cross(vector01, vector02);
		normal.Normalize();

		normal *= magnitude;


		float3 center = (vertex0 + vertex1 + vertex2) / 3;
		//Provisional placement (it displays the normal in the first vertex)
		glVertex3f(center.x, center.y, center.z);
		glVertex3f((center.x + normal.x), (center.y + normal.y), (center.z + normal.z));

	}

	glEnd();

	glLineWidth(2.0f);
}

void RenderMesh::DrawBuffers(ResourceMesh* m)
{
	unsigned int texIDtoBind = 0;
	glBindBuffer(GL_ARRAY_BUFFER, m->idVertex);			//this is for printing the index
	glVertexPointer(3, GL_FLOAT, 0, NULL);				//Null => somehow OpenGL knows what you're talking about


	if (!m->normals.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, m->idNormals);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}

	if (!m->texCoords.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, m->idTexCoords);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->idIndex);	//Because this Bind is after the vertex bind, OpenGl knows these two are in order & connected. *Magic*	

	if (material != nullptr)
	{
		if (material->usingCkeckers)
		{
			texIDtoBind = material->GetCheckersID();
		}
		else if (material->HasTexture())
		{
			texIDtoBind = material->GetTextureID();
		}
	}
	if (texIDtoBind != 0)
	{
		glBindTexture(GL_TEXTURE_2D, texIDtoBind);
	}

	glDrawElements(GL_TRIANGLES, m->indices.size(), GL_UNSIGNED_INT, NULL);	//End of "bind addition" here...
	if (texIDtoBind != 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}
