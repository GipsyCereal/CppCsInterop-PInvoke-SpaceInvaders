#pragma once
#include <vector>
#include <string>

namespace CppLibrary
{
	//--------- MACRO ---------//
#define SAFE_DELETE(p) if (p) { delete (p); (p) = nullptr; }

//--------- TYPES ---------//
	typedef void(*SoundCallback)();

	//--------- STRUCTS ---------//
	struct Vector2
	{
		float x, y;

		Vector2() :x(.0f), y(.0f)
		{}
		Vector2(float _x, float _y)
			:x(_x), y(_y)
		{}

		Vector2& operator+= (const Vector2& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;
			return *this;
		}

		Vector2 operator+(const Vector2& rhs) const
		{
			return Vector2(this->x + rhs.x, this->y + rhs.y);
		}
	};

	//--------- CLASSES ---------//
	enum ObjectContainerType
	{
		ALIEN,
		PROJECTILE
	};

	class GameObject
	{
	public:
		GameObject(const Vector2& position, const Vector2& size)
			:m_Position(position), m_Size(size)
		{}

		Vector2 GetPosition() const
		{
			return m_Position;
		}

		void MovePosition(const Vector2& p)
		{
			m_Position += p;
		}

		Vector2 GetSize() const
		{
			return m_Size;
		}

		bool IsColliding(GameObject const* const pOtherObject) const
		{
			auto otherPosition = pOtherObject->GetPosition();
			auto otherSize = pOtherObject->GetSize();
			auto otherLeft = otherPosition.x - (otherSize.x / 2.0f);
			auto otherRight = otherLeft + otherSize.x;
			auto thisLeft = m_Position.x - (m_Size.x / 2.0f);
			auto thisRight = thisLeft + m_Size.x;

			// If one rectangle is on left side of the other
			if (thisRight < otherLeft || thisLeft > otherRight)
				return false;

			auto otherTop = otherPosition.y + (otherSize.y / 2.0f);
			auto otherBottom = otherTop - otherSize.y;
			auto thisTop = m_Position.y + (m_Size.y / 2.0f);
			auto thisBottom = thisTop - m_Size.y;

			// If one rectangle is under the other
			if (thisBottom > otherTop || thisTop < otherBottom)
				return false;

			return true;
		}

	private:
		Vector2 m_Position = {};
		Vector2 m_Size = {}; //Bounding Box
	};

	class GameManager final
	{
	public:
		GameManager()
		{
			//Create list of aliens at start
			InitialState();
		}
		~GameManager()
		{
			ClearAliens();
			ClearProjectiles();
			SAFE_DELETE(m_pPlayer);
		}

		void Update(float deltaTime)
		{
			//Y Movement Aliens
			m_VerticalMovementTimer += deltaTime;
			if (m_VerticalMovementTimer >= m_VerticalMovementTime)
			{
				m_VerticalMovementTimer = 0.f;
				for (auto& pAlien : m_vpAliens)
					pAlien->MovePosition(Vector2(0, -m_VerticalDisplacement));
			}

			//X Movement Aliens
			float relativeXDisplacement = m_GroupDirection * m_HorizontalDisplacement * deltaTime;
			float leftGroup = m_GroupLeft + relativeXDisplacement;
			float rightGroup = m_GroupRight + relativeXDisplacement;
			for (auto& pAlien : m_vpAliens)
			{
				if (leftGroup <= m_LeftLimit || rightGroup >= m_RightLimit)
				{
					m_GroupDirection *= -1.f;
					relativeXDisplacement = 0.0f;
					break;
				}

				pAlien->MovePosition(Vector2(relativeXDisplacement, 0.0f));
			}
			m_GroupLeft += relativeXDisplacement;
			m_GroupRight += relativeXDisplacement;

			//Movement & OutOfBounds Projectiles
			for (auto i = m_vpProjectiles.begin(); i != m_vpProjectiles.end();)
			{
				(*i)->MovePosition(Vector2(0, m_ProjectileSpeed * deltaTime));

				const float maxYPos = 40.0f;
				if ((*i)->GetPosition().y >= maxYPos)
				{
					SAFE_DELETE(*i);
					i = m_vpProjectiles.erase(i);
				}
				else
					++i;
			}

			//Collision - Projectiles vs Aliens
			for (auto alienIt = m_vpAliens.begin(); alienIt != m_vpAliens.end();)
			{
				bool hadCollision = false;
				for (auto projectileIt = m_vpProjectiles.begin(); projectileIt != m_vpProjectiles.end();)
				{
					if ((*alienIt)->IsColliding((*projectileIt)))
					{
						hadCollision = true;
						SAFE_DELETE(*alienIt);
						alienIt = m_vpAliens.erase(alienIt);
						SAFE_DELETE(*projectileIt);
						m_vpProjectiles.erase(projectileIt);
						if (m_pKillSoundFunction) //Play Sound
							m_pKillSoundFunction();
						break;
					}
					++projectileIt;
				}

				if (!hadCollision)
					++alienIt;
			}

			//Auto Reset - Death OR Killed all aliens
			const float deathYLimit = m_pPlayer->GetPosition().y + 5;
			if (m_vpAliens.size() <= 0 || m_vpAliens.at(0)->GetPosition().y <= deathYLimit)
				InitialState();
		}

		const std::vector<GameObject*>& GetGameObjectContainer(ObjectContainerType type) const
		{
			switch (type)
			{
			case ALIEN:
				return m_vpAliens;
				break;
			case PROJECTILE:
				return m_vpProjectiles;
				break;
			}
			return {};
		}

		GameObject const* const GetPlayer() const
		{
			return m_pPlayer;
		}

		void SetName(char* p)
		{
			m_Name = std::string(p) + "_InUnmanagedMemory";
		}

		const char* GetName() const
		{
			return m_Name.c_str();
		}

		void MovePlayer(const Vector2& direction, float deltaTime)
		{
			auto relativeDisplacement = Vector2(direction.x * m_MovementSpeed * deltaTime, direction.y * m_MovementSpeed * deltaTime);
			const auto absolutePosition = m_pPlayer->GetPosition() + relativeDisplacement;
			if (absolutePosition.x <= m_LeftLimit || absolutePosition.x >= m_RightLimit)
				relativeDisplacement.x = 0.0f;
			m_pPlayer->MovePosition(relativeDisplacement);
		}

		void SpawnProjectile(const Vector2& position)
		{
			m_vpProjectiles.push_back(new GameObject(position, Vector2(0.5f, 0.5f)));
			if (m_pFireSoundFunction) //Play Sound
				m_pFireSoundFunction();
		}

		void SetShootCallback(SoundCallback pFnc)
		{
			m_pFireSoundFunction = pFnc;
		}

		void SetKillCallback(SoundCallback pFnc)
		{
			m_pKillSoundFunction = pFnc;
		}

	private:
		void ClearAliens()
		{
			for (int i = 0; i < (int)m_vpAliens.size(); ++i)
				SAFE_DELETE(m_vpAliens.at(i));
			m_vpAliens.clear();
		}

		void ClearProjectiles()
		{
			for (int i = 0; i < (int)m_vpProjectiles.size(); ++i)
				SAFE_DELETE(m_vpProjectiles.at(i));
			m_vpProjectiles.clear();
		}

		void InitialState()
		{
			ClearAliens();
			ClearProjectiles();

			const unsigned int amountColumns = 7;
			const unsigned int amountRows = 5;
			const float offset = 2;
			const float size = 2;
			for (int c = 0; c < amountColumns; ++c)
			{
				for (int r = 0; r < amountRows; ++r)
				{
					Vector2 position = Vector2(c * (offset + size), r * (offset + size));
					m_vpAliens.push_back(new GameObject(position, Vector2(size, size)));
				}
			}
			m_GroupLeft = 0.0f - (size / 2.0f);
			m_GroupRight = (amountColumns - 1) * (offset + size) + (size / 2.0f);

			SAFE_DELETE(m_pPlayer);
			const float playerYPosition = -47.5f;
			m_pPlayer = new GameObject(Vector2((amountColumns - 1.0f) * (offset + size) / 2.0f, playerYPosition), Vector2(size, size));
		}

	private:
		std::vector<GameObject*> m_vpAliens = {};
		std::vector<GameObject*> m_vpProjectiles = {};
		GameObject* m_pPlayer = nullptr;
		std::string m_Name = {};

		float m_VerticalMovementTimer = 0.f;
		const float m_VerticalMovementTime = 3.f;
		const float m_VerticalDisplacement = 2.0f;
		const float m_HorizontalDisplacement = 5.0f;
		float m_GroupLeft = 0.0f;
		float m_GroupRight = 0.0f;
		float m_GroupDirection = -1.0f;

		const float m_LeftLimit = -10.0f;
		const float m_RightLimit = 25.0f;

		const float m_MovementSpeed = 15.0f;
		const float m_ProjectileSpeed = 20.0f;

		SoundCallback m_pFireSoundFunction = nullptr;
		SoundCallback m_pKillSoundFunction = nullptr;
	};
}
