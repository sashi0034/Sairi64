#pragma once

class DearImGuiAddon : public ::s3d::IAddon
{
public:
	bool init() override;

	bool update() override;

	void draw() const override;

	~DearImGuiAddon() override;

private:
	bool m_firstFrame = true;
};

class ImS3dTexture
{
public:
	explicit ImS3dTexture(const Texture& texture);
	Texture& GetTexture() const;
	ImTextureID GetId() const;

private:
	class Impl;
	std::shared_ptr<Impl> m_impl{};
};
