
class Pickup
{
public:
	Pickup();
	~Pickup();

	Pickup(const Pickup& other) = delete;
	Pickup& operator=(const Pickup& other) = delete;
	Pickup(Pickup&& other) = delete;
	Pickup& operator=(Pickup&& other) = delete;

	void Update(float elapsedSec);
	void Draw() const;

private:
	
};