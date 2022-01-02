class Window;

class Win_Play : public Window
{
public:

	Win_Play(bool _active);
	~Win_Play();

	void Init() override;
	void Draw() override;

	void CleanUp() override;

private:

};
