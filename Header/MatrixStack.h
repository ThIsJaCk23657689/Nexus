#pragma once

#include <glm/glm.hpp>
#include "Logger.h"

namespace Nexus {
	class MatrixStack {
	public:
		MatrixStack() : Index(0), Capacity(32) {
			this->Stack = new glm::mat4[this->Capacity];
			this->Stack[0] = glm::mat4(1.0f);
		}

		~MatrixStack() {
			delete[] this->Stack;
		}

		void Push() {
			if (this->Index == this->Capacity - 1) {
				doubleCapacity();
			}
			this->Stack[this->Index + 1] = this->Stack[this->Index];
			this->Index++;
		}
		
		void Pop() {
			if (this->IsEmpty()) {
				Logger::Message(LOG_ERROR, "The matrix stack is empty.");
				return;
			}
			this->Index--;
		}

		void Save(glm::mat4 matrix) const {
			this->Stack[this->Index] = matrix;
		}
		
		int GetSize() const {
			return this->Index + 1;
		}
		
		bool IsEmpty() const {
			return (this->Index == 0);
		}
		
		glm::mat4 Top() const {
			return this->Stack[this->Index];
		}

	private:
		int Index;
		int Capacity;
		glm::mat4* Stack;

		void doubleCapacity() {
			this->Capacity *= 2;

			glm::mat4* newStack = new glm::mat4[this->Capacity];
			for (int i = 0; i < this->Capacity / 2; i++) {
				newStack[i] = this->Stack[i];
			}

			delete[] this->Stack;
			this->Stack = newStack;
		}
	};
}
