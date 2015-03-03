#include "stdafx.h"
#include "MCamera.h"
BOOL MCamera::Load_3DS_Object(int pos, int filePos)
{
	// Load the *.3DS file.  We just pass in an address to our t3DModel structure and the file name string we want to load
	g_Load3ds.Reset();
	return g_Load3ds.Import3DS(&g_3DModel[pos], modelParameter[filePos].modelPath, m_frame);//FILE_TEXTURE_3DS);			// Load our .3DS file into our model structure
	// Depending on how many textures we found, load each one
	/* no textures */
	//for(int i = 0; i < g_3DModel[pos].numOfMaterials; i++)
	//{
	//	// Check to see if there is a file name to load in this material
	//	if(strlen(g_3DModel[pos].pMaterials[i].strFile) > 0)
	//	{
	//		// We pass in our global texture array, the name of the texture, and an ID to reference it.	
	//		Textures.LoadTexture(g_3DModel[pos].pMaterials[i].strFile, g_Texture[i]);				
	//	}

	//	// Set the texture ID for this material
	//	g_3DModel[pos].pMaterials[i].texureId = i;
	//}
}
void MCamera::Draw_3DS_Object(int pos, int pX, int pY, int pZ, int pSize)
{
	// We want the model to rotate around the axis so we give it a rotation
	// value, then increase/decrease it. You can rotate right of left with the arrow keys.
	glPushMatrix();
	glScaled(pSize, pSize, pSize);
	glTranslatef(pX, pY, pZ);

	//	glRotatef(g_RotateX, 0, 1.0f, 0);							// Rotate the object around the Y-Axis	
	//	if (g_RotateObject)	{	g_RotateX += g_RotationSpeed;	}	// Increase the speed of rotation

	// We have a model that has a certain amount of objects and textures.  We want to go through each object 
	// in the model, bind it's texture map to it, then render it by going through all of it's faces (Polygons).  	

	// Since we know how many objects our model has, go through each of them.
	for(int i = 0; i < g_3DModel[pos].numOfObjects; i++)
	{
		// Make sure we have valid objects just in case. (size() is in the vector class)
		if(g_3DModel[pos].pObject.size() <= 0) break;

		// Get the current object that we are displaying
		t3DObject *pObject = &g_3DModel[pos].pObject[i];

		// Check to see if this object has a texture map, if so bind the texture to it.
		if(pObject->bHasTexture) 
		{
			// Turn on texture mapping and turn off color
			glEnable(GL_TEXTURE_2D);

			// Reset the color to normal again
			//glColor3ub(255, 255, 255);
			glColor3fv(g_3DModel[pos].baseColor);

			// Bind the texture map to the object by it's materialID
			glBindTexture(GL_TEXTURE_2D, g_Texture[pObject->materialID]);
		} 
		else 
		{
			// Turn off texture mapping and turn on color
			glDisable(GL_TEXTURE_2D);

			// Reset the color to normal again
			//glColor3ub(255, 255, 255);
			glColor3fv(g_3DModel[pos].baseColor);
		}

		// This determines if we are in wireframe or normal mode
		glBegin(g_ViewMode);					// Begin drawing with our selected mode (triangles or lines)

		// Go through all of the faces (polygons) of the object and draw them
		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			// Go through each corner of the triangle and draw it.
			for(int whichVertex = 0; whichVertex < 3; whichVertex++)
			{
				// Get the index for each point of the face
				int index = pObject->pFaces[j].vertIndex[whichVertex];

				// Give OpenGL the normal for this vertex.
				glNormal3f(pObject->pNormals[index].x, pObject->pNormals[index].y, pObject->pNormals[index].z);

				// If the object has a texture associated with it, give it a texture coordinate.
				if(pObject->bHasTexture) 
				{

					// Make sure there was a UVW map applied to the object or else it won't have tex coords.
					if(pObject->pTexVerts) 
					{
						glTexCoord2f(pObject->pTexVerts[ index ].x, pObject->pTexVerts[ index ].y);
					}
				} 
				else 
				{
					// if the size is at > 1 and material ID != -1, then it is a valid material.
					if(g_3DModel[pos].pMaterials.size() && pObject->materialID >= 0) 
					{
						// Get and set the color that the object is, since it must not have a texture
						BYTE *pColor = g_3DModel[pos].pMaterials[pObject->materialID].color;

						// Assign the current color to this model
						glColor3ub(pColor[0], pColor[1], pColor[2]);
					}
				}

				// Pass in the current vertex of the object (Corner of current face)
				glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
			}
		}

		glEnd();			// End the model drawing
	}

	glPopMatrix();
}
void MCamera::Destroy_3DS_Object(int pos)
{
	g_Load3ds.Destroy3DS(&g_3DModel[pos]);
}