#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>

#define GLM_FORCE_RADIANS 
#define TWOPI 2*3.141592653589793238


#include <glm/glm.hpp>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


	class objloader {
	public:
		//! Constructor
		//!
		objloader();

		//! Destructor
		//!
		~objloader();

		
		bool load(const char *filename);

		std::vector<glm::vec3> const &getVertices() const;
		std::vector<glm::vec3> const &getNormals() const;
		std::vector<int> const &getVertexIndices() const;
		std::vector<glm::vec2> const &getTexCoordinates() const;
		
		 

		void computeNormals(std::vector<glm::vec3> const &vertices,
			std::vector<int> const &indices,
			std::vector<glm::vec3> &normals);
	private:
		std::vector<glm::vec3> mVertices;
		std::vector<glm::vec2> mTexCoordinates;
		std::vector<glm::vec3> mNormals;
		std::vector<int> vIndices;
		std::vector<int> tIndices;
		//std::vector<unsigned int> vIndices;
		
	};

#endif

