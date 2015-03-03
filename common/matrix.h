#ifndef matrix_H
#define matrix_H

#include <memory.h>
#include "util.h"
#include "vector3.h"

class matrix
{
public:
	union
	{
		float m[16];
		struct
		{
			float x[4];
			float y[4];
			float z[4];
			float w[4];
		};
	};

	matrix()
	{
	}
	~matrix()
	{
	}

	matrix(const float *p)
	{
		fill(p);
	}
	matrix(const double *p)
	{
		fill(p);
	}

	matrix& fill(const float *p)
	{
		m[0] = p[0];
		m[1] = p[1];
		m[2] = p[2];
		m[3] = p[3];
		m[4] = p[4];
		m[5] = p[5];
		m[6] = p[6];
		m[7] = p[7];
		m[8] = p[8];
		m[9] = p[9];
		m[10] = p[10];
		m[11] = p[11];
		m[12] = p[12];
		m[13] = p[13];
		m[14] = p[14];
		m[15] = p[15];

		return *this;
	}
	matrix& fill(const double *p)
	{
		m[0] = (float)p[0];
		m[1] = (float)p[1];
		m[2] = (float)p[2];
		m[3] = (float)p[3];
		m[4] = (float)p[4];
		m[5] = (float)p[5];
		m[6] = (float)p[6];
		m[7] = (float)p[7];
		m[8] = (float)p[8];
		m[9] = (float)p[9];
		m[10] = (float)p[10];
		m[11] = (float)p[11];
		m[12] = (float)p[12];
		m[13] = (float)p[13];
		m[14] = (float)p[14];
		m[15] = (float)p[15];

		return *this;
	}

	vector3& xvec() const
	{
		return *(vector3*)x;
	}
	vector3& yvec() const
	{
		return *(vector3*)y;
	}
	vector3& zvec() const
	{
		return *(vector3*)z;
	}

	vector3& pos() const
	{
		return *(vector3*)w;
	}

	matrix& identity()
	{
		m[0] = 1;
		m[1] = 0;
		m[2] = 0;
		m[3] = 0;
		m[4] = 0;
		m[5] = 1;
		m[6] = 0;
		m[7] = 0;
		m[8] = 0;
		m[9] = 0;
		m[10] = 1;
		m[11] = 0;
		m[12] = 0;
		m[13] = 0;
		m[14] = 0;
		m[15] = 1;

		return *this;
	}

	matrix& keep3x3()
	{
		m[3] = 0;
		m[7] = 0;
		m[11] = 0;
		m[12] = 0;
		m[13] = 0;
		m[14] = 0;
		m[15] = 1;

		return *this;
	}

	matrix& transpose()
	{
		util::swap(m[1], m[4]);
		util::swap(m[2], m[8]);
		util::swap(m[3], m[12]);
		util::swap(m[6], m[9]);
		util::swap(m[7], m[13]);
		util::swap(m[11], m[14]);

		return *this;
	}

	matrix& translate(const vector3 &v)
	{
		m[0] = 1;
		m[1] = 0;
		m[2] = 0;
		m[3] = 0;
		m[4] = 0;
		m[5] = 1;
		m[6] = 0;
		m[7] = 0;
		m[8] = 0;
		m[9] = 0;
		m[10] = 1;
		m[11] = 0;
		m[12] = v.x;
		m[13] = v.y;
		m[14] = v.z;
		m[15] = 1;

		return *this;
	}

	matrix& translate(const float x, const float y, const float z)
	{
		m[0] = 1;
		m[1] = 0;
		m[2] = 0;
		m[3] = 0;
		m[4] = 0;
		m[5] = 1;
		m[6] = 0;
		m[7] = 0;
		m[8] = 0;
		m[9] = 0;
		m[10] = 1;
		m[11] = 0;
		m[12] = x;
		m[13] = y;
		m[14] = z;
		m[15] = 1;

		return *this;
	}

	matrix& scale(const float x, const float y, const float z, const float w)
	{
		m[0] = x;
		m[1] = 0;
		m[2] = 0;
		m[3] = 0;
		m[4] = 0;
		m[5] = y;
		m[6] = 0;
		m[7] = 0;
		m[8] = 0;
		m[9] = 0;
		m[10] = z;
		m[11] = 0;
		m[12] = 0;
		m[13] = 0;
		m[14] = 0;
		m[15] = w;

		return *this;
	}

	// does not do the translation portion
	// follow with a translate(-from.x, -from.y, -from.z)
	matrix& lookAt(const vector3 &from, const vector3 &to, const vector3 &up)
	{
		identity();
		if (from == to)
			return *this;

		vector3 f, s, u;
		f = (to - from).normalize();
		s = Cross(f, up).normalize();
		u = Cross(s, f);//.normalize();

		m[0] = s.v[0];
		m[4] = s.v[1];
		m[8] = s.v[2];

		m[1] = u.v[0];
		m[5] = u.v[1];
		m[9] = u.v[2];

		m[2] = -f.v[0];
		m[6] = -f.v[1];
		m[10] = -f.v[2];

		return *this;
	}

	// infinite projection
	matrix& perspective(float fov, float znear, float aspect)
	{
		m[4] = m[8] = m[12] = m[1] = m[9] = m[13] = m[2] = m[6] = m[3] = m[7] = m[15] = 0;
		m[0] = (1.0f / tan(fov * .5f * PIOVER180)) / aspect;
		m[5] = (1.0f / tan(fov * .5f * PIOVER180));
		m[14] = -2.0f * znear; // near clip
		m[10] = m[11] = -1.0f;

		return *this;
	}

	matrix& ortho(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		float tx = -(right + left) / (right - left);
		float ty = -(top + bottom) / (top - bottom);
		float tz = -(zFar + zNear) / (zFar - zNear);

		m[0] = 2.0f / (right - left);
		m[1] = 0;
		m[2] = 0;
		m[3] = 0;

		m[4] = 0;
		m[5] = 2.0f / (top - bottom);
		m[6] = 0;
		m[7] = 0;

		m[8] = 0;
		m[9] = 0;
		m[10] = -2.0f / (zFar - zNear);
		m[11] = 0;

		m[12] = tx;
		m[13] = ty;
		m[14] = tz;
		m[15] = 1.0f;

		return *this;
	}

	// need a fast orthogonal inverse
	matrix& inverse()
	{
		matrix minv;
		
		float r1[8], r2[8], r3[8], r4[8];
		float *s[4], *tmprow;
		
		s[0] = &r1[0];
		s[1] = &r2[0];
		s[2] = &r3[0];
		s[3] = &r4[0];
		
		register int i,j,p,jj;
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				s[i][j] = m[i*4+j];
				if(i==j) s[i][j+4] = 1.0;
				else     s[i][j+4] = 0.0;
			}
		}
		float scp[4];
		for(i=0;i<4;i++)
		{
			scp[i] = float(fabs(s[i][0]));
			for(j=1;j<4;j++)
				if(float(fabs(s[i][j])) > scp[i]) scp[i] = float(fabs(s[i][j]));
				if(scp[i] == 0.0)
					return *this;
		}
		
		int pivot_to;
		float scp_max;
		for(i=0;i<4;i++)
		{
			// select pivot row
			pivot_to = i;
			scp_max = float(fabs(s[i][i]/scp[i]));
			// find out which row should be on top
			for(p=i+1;p<4;p++)
				if(float(fabs(s[p][i]/scp[p])) > scp_max)
				{ scp_max = float(fabs(s[p][i]/scp[p])); pivot_to = p; }
				// Pivot if necessary
				if(pivot_to != i)
				{
					tmprow = s[i];
					s[i] = s[pivot_to];
					s[pivot_to] = tmprow;
					float tmpscp;
					tmpscp = scp[i];
					scp[i] = scp[pivot_to];
					scp[pivot_to] = tmpscp;
				}

				float mji;
				// perform gaussian elimination
				for(j=i+1;j<4;j++)
				{
					mji = s[j][i]/s[i][i];
					s[j][i] = 0.0;
					for(jj=i+1;jj<8;jj++)
						s[j][jj] -= mji*s[i][jj];
				}
		}
		if(s[3][3] == 0.0)
			return *this;


		float mij;
		for(i=3;i>0;i--)
		{
			for(j=i-1;j > -1; j--)
			{
				mij = s[j][i]/s[i][i];
				for(jj=j+1;jj<8;jj++)
					s[j][jj] -= mij*s[i][jj];
			}
		}

		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				minv.m[i*4+j] = s[i][j+4] / s[i][i];

		memcpy(m, minv.m, sizeof(float) * 16);

		return *this;
	}

	vector3 operator * (const vector3 &p) const
	{
		float invw;
		invw = 1.0f / (p.v[0] * m[3] + p.v[1] * m[7] + p.v[2] * m[11] + m[15]);

		vector3 r;
		r.v[0] = (p.v[0] * m[0] + p.v[1] * m[4] + p.v[2] * m[8] + m[12]) * invw;
		r.v[1] = (p.v[0] * m[1] + p.v[1] * m[5] + p.v[2] * m[9] + m[13]) * invw;
		r.v[2] = (p.v[0] * m[2] + p.v[1] * m[6] + p.v[2] * m[10] + m[14]) * invw;

		return r;
	}
	void transform4x4(vector3 &dest, const vector3 &src0) const
	{
		float invw;
		invw = 1.0f / (src0.v[0] * m[3] + src0.v[1] * m[7] + src0.v[2] * m[11] + m[15]);

		dest.v[0] = (src0.v[0] * m[0] + src0.v[1] * m[4] + src0.v[2] * m[8] + m[12]) * invw;
		dest.v[1] = (src0.v[0] * m[1] + src0.v[1] * m[5] + src0.v[2] * m[9] + m[13]) * invw;
		dest.v[2] = (src0.v[0] * m[2] + src0.v[1] * m[6] + src0.v[2] * m[10] + m[14]) * invw;
	}

	vector3 transform4x3(const vector3 &p) const
	{
		vector3 r;
		r.v[0] = p.v[0] * m[0] + p.v[1] * m[4] + p.v[2] * m[8] + m[12];
		r.v[1] = p.v[0] * m[1] + p.v[1] * m[5] + p.v[2] * m[9] + m[13];
		r.v[2] = p.v[0] * m[2] + p.v[1] * m[6] + p.v[2] * m[10] + m[14];

		return r;
	}
	void transform4x3(vector3 &dest, const vector3 &src0) const
	{
		dest.v[0] = src0.v[0] * m[0] + src0.v[1] * m[4] + src0.v[2] * m[8] + m[12];
		dest.v[1] = src0.v[0] * m[1] + src0.v[1] * m[5] + src0.v[2] * m[9] + m[13];
		dest.v[2] = src0.v[0] * m[2] + src0.v[1] * m[6] + src0.v[2] * m[10] + m[14];
	}

	vector3 transform3x3(const vector3 &p) const
	{
		vector3 r;
		r.v[0] = p.v[0] * m[0] + p.v[1] * m[4] + p.v[2] * m[8];
		r.v[1] = p.v[0] * m[1] + p.v[1] * m[5] + p.v[2] * m[9];
		r.v[2] = p.v[0] * m[2] + p.v[1] * m[6] + p.v[2] * m[10];

		return r;
	}

	// returns false if outside NDC box
	bool transformVectorNDC(vector3 &p) const
	{
		float x = p.v[0] * m[0] + p.v[1] * m[4] + p.v[2] * m[8] + m[12];
		float y = p.v[0] * m[1] + p.v[1] * m[5] + p.v[2] * m[9] + m[13];
		float z = p.v[0] * m[2] + p.v[1] * m[6] + p.v[2] * m[10] + m[14];
		float w = p.v[0] * m[3] + p.v[1] * m[7] + p.v[2] * m[11] + m[15];

		if (x < -w || x > w)
			return false;
		if (y < -w || y > w)
			return false;
		if (z < -w || z > w)
			return false;

		float invw = 1.0f / w;
		p.x = x * invw;
		p.y = y * invw;
		p.z = z * invw;

		return true;
	}

	matrix operator * (const matrix &p) const
	{
		matrix r;
		r.m[0] = m[0] * p.m[0] + m[4] * p.m[1] + m[8] * p.m[2] + m[12] * p.m[3];
		r.m[1] = m[1] * p.m[0] + m[5] * p.m[1] + m[9] * p.m[2] + m[13] * p.m[3];
		r.m[2] = m[2] * p.m[0] + m[6] * p.m[1] + m[10] * p.m[2] + m[14] * p.m[3];
		r.m[3] = m[3] * p.m[0] + m[7] * p.m[1] + m[11] * p.m[2] + m[15] * p.m[3];
		r.m[4] = m[0] * p.m[4] + m[4] * p.m[5] + m[8] * p.m[6] + m[12] * p.m[7];
		r.m[5] = m[1] * p.m[4] + m[5] * p.m[5] + m[9] * p.m[6] + m[13] * p.m[7];
		r.m[6] = m[2] * p.m[4] + m[6] * p.m[5] + m[10] * p.m[6] + m[14] * p.m[7];
		r.m[7] = m[3] * p.m[4] + m[7] * p.m[5] + m[11] * p.m[6] + m[15] * p.m[7];
		r.m[8] = m[0] * p.m[8] + m[4] * p.m[9] + m[8] * p.m[10] + m[12] * p.m[11];
		r.m[9] = m[1] * p.m[8] + m[5] * p.m[9] + m[9] * p.m[10] + m[13] * p.m[11];
		r.m[10] = m[2] * p.m[8] + m[6] * p.m[9] + m[10] * p.m[10] + m[14] * p.m[11];
		r.m[11] = m[3] * p.m[8] + m[7] * p.m[9] + m[11] * p.m[10] + m[15] * p.m[11];
		r.m[12] = m[0] * p.m[12] + m[4] * p.m[13] + m[8] * p.m[14] + m[12] * p.m[15];
		r.m[13] = m[1] * p.m[12] + m[5] * p.m[13] + m[9] * p.m[14] + m[13] * p.m[15];
		r.m[14] = m[2] * p.m[12] + m[6] * p.m[13] + m[10] * p.m[14] + m[14] * p.m[15];
		r.m[15] = m[3] * p.m[12] + m[7] * p.m[13] + m[11] * p.m[14] + m[15] * p.m[15];
		return r;
	}
	matrix& operator *= (const matrix &p)
	{
		matrix r;
		r.m[0] = m[0] * p.m[0] + m[4] * p.m[1] + m[8] * p.m[2] + m[12] * p.m[3];
		r.m[1] = m[1] * p.m[0] + m[5] * p.m[1] + m[9] * p.m[2] + m[13] * p.m[3];
		r.m[2] = m[2] * p.m[0] + m[6] * p.m[1] + m[10] * p.m[2] + m[14] * p.m[3];
		r.m[3] = m[3] * p.m[0] + m[7] * p.m[1] + m[11] * p.m[2] + m[15] * p.m[3];
		r.m[4] = m[0] * p.m[4] + m[4] * p.m[5] + m[8] * p.m[6] + m[12] * p.m[7];
		r.m[5] = m[1] * p.m[4] + m[5] * p.m[5] + m[9] * p.m[6] + m[13] * p.m[7];
		r.m[6] = m[2] * p.m[4] + m[6] * p.m[5] + m[10] * p.m[6] + m[14] * p.m[7];
		r.m[7] = m[3] * p.m[4] + m[7] * p.m[5] + m[11] * p.m[6] + m[15] * p.m[7];
		r.m[8] = m[0] * p.m[8] + m[4] * p.m[9] + m[8] * p.m[10] + m[12] * p.m[11];
		r.m[9] = m[1] * p.m[8] + m[5] * p.m[9] + m[9] * p.m[10] + m[13] * p.m[11];
		r.m[10] = m[2] * p.m[8] + m[6] * p.m[9] + m[10] * p.m[10] + m[14] * p.m[11];
		r.m[11] = m[3] * p.m[8] + m[7] * p.m[9] + m[11] * p.m[10] + m[15] * p.m[11];
		r.m[12] = m[0] * p.m[12] + m[4] * p.m[13] + m[8] * p.m[14] + m[12] * p.m[15];
		r.m[13] = m[1] * p.m[12] + m[5] * p.m[13] + m[9] * p.m[14] + m[13] * p.m[15];
		r.m[14] = m[2] * p.m[12] + m[6] * p.m[13] + m[10] * p.m[14] + m[14] * p.m[15];
		r.m[15] = m[3] * p.m[12] + m[7] * p.m[13] + m[11] * p.m[14] + m[15] * p.m[15];
		memcpy(m, r.m, sizeof(float) * 16);
		return *this;
	}

	bool operator == (const matrix &p) const
	{
		for (int i = 0; i < 16; i++)
		{
			if (m[i] != p.m[i])
				return false;
		}
		return true;
	}
	bool operator != (const matrix &p) const
	{
		return !(*this == p);
	}

	// side is an opengl define
	matrix& cubemapmatrix(int side)
	{
		switch (side)
		{
		case 0x8515: // +X
			m[0] =  0; m[4] = 0; m[8]  = -1; m[12] = 0;
			m[1] =  0; m[5] = -1; m[9]  = 0; m[13] = 0;
			m[2] = -1; m[6] = 0; m[10] = 0; m[14] = 0;
			m[3] =  0; m[7] = 0; m[11] = 0; m[15] = 1;
			break;
		case 0x8516: // -X
			m[0] = 0; m[4] = 0; m[8]  = 1; m[12] = 0;
			m[1] = 0; m[5] = -1; m[9]  =  0; m[13] = 0;
			m[2] = 1; m[6] = 0; m[10] =  0; m[14] = 0;
			m[3] = 0; m[7] = 0; m[11] =  0; m[15] = 1;
			break;
		case 0x8517: // +Y
			m[0] = 1; m[4] =  0; m[8]  =  0; m[12] = 0;
			m[1] =  0; m[5] =  0; m[9]  = 1; m[13] = 0;
			m[2] =  0; m[6] = -1; m[10] =  0; m[14] = 0;
			m[3] =  0; m[7] =  0; m[11] =  0; m[15] = 1;
			break;
		case 0x8518: // -Y
			m[0] = 1; m[4] = 0; m[8]  = 0; m[12] = 0;
			m[1] =  0; m[5] = 0; m[9]  = -1; m[13] = 0;
			m[2] =  0; m[6] = 1; m[10] = 0; m[14] = 0;
			m[3] =  0; m[7] = 0; m[11] = 0; m[15] = 1;
			break;
		case 0x8519: // +Z
			m[0] = 1; m[4] = 0; m[8]  =  0; m[12] = 0;
			m[1] =  0; m[5] = -1; m[9]  =  0; m[13] = 0;
			m[2] =  0; m[6] = 0; m[10] = -1; m[14] = 0;
			m[3] =  0; m[7] = 0; m[11] =  0; m[15] = 1;
			break;
		case 0x851A: // -Z
			m[0] = -1; m[4] = 0; m[8]  = 0; m[12] = 0;
			m[1] = 0; m[5] = -1; m[9]  = 0; m[13] = 0;
			m[2] = 0; m[6] = 0; m[10] = 1; m[14] = 0;
			m[3] = 0; m[7] = 0; m[11] = 0; m[15] = 1;
			break;
		}

		return *this;
	}

	vector3 getRotationAnglesXYZ() const
	{
		vector3 rot;
		if (z[0] == -1.0f)
		{
			rot.z = 0;
			float w = atan2(x[1], x[2]);

			rot.y = PI * .5f;
			rot.x = rot.z + w;
		}
		else if (z[0] == 1.0f)
		{
			rot.z = 0;
			float w = atan2(x[1], x[2]);

			rot.y = PI * -.5f;
			rot.x = -rot.z + w;
		}
		else
		{
			// there are two solutions, we're just going to find the first
			rot.y = -asin(z[0]);
			float invCosTheta = 1.0f / cos(rot.y);

			rot.x = atan2(z[1] * invCosTheta, z[2] * invCosTheta);
			rot.z = atan2(y[0] * invCosTheta, x[0] * invCosTheta);
		}

		return rot;
	}
};

#endif