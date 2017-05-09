﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2017 Ryo Suzuki
//	Copyright (c) 2016-2017 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "../Siv3DEngine.hpp"
# include "ITexture.hpp"
# include "../Renderer2D/IRenderer2D.hpp"
# include <Siv3D/Texture.hpp>
# include <Siv3D/TextureRegion.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/FloatRect.hpp>

namespace s3d
{
	template <>
	AssetHandle<Texture::Handle>::~AssetHandle()
	{
		if (!Siv3DEngine::isActive())
		{
			return;
		}

		if (auto p = Siv3DEngine::GetTexture())
		{
			p->release(m_id);
		}
	}

	Texture::Texture(BackBuffer)
		: m_handle(std::make_shared<TextureHandle>(Siv3DEngine::GetTexture()->createFromBackBuffer()))
	{

	}

	Texture::Texture(Render, const Size& size, const uint32 multisampleCount)
		: m_handle(std::make_shared<TextureHandle>(Siv3DEngine::GetTexture()->createRT(size, multisampleCount)))
	{

	}

	Texture::Texture()
		: m_handle(std::make_shared<TextureHandle>())
	{

	}

	Texture::Texture(const Image& image, const TextureDesc desc)
		: m_handle(std::make_shared<TextureHandle>(Siv3DEngine::GetTexture()->create(image, desc)))
	{
		//ASSET_CREATION;
	}

	Texture::Texture(const FilePath& path, const TextureDesc desc)
		: Texture(Image(path), desc)
	{

	}

	Texture::Texture(IReader&& reader, const TextureDesc desc)
		: Texture(Image(std::move(reader)), desc)
	{

	}

	Texture::Texture(const FilePath& rgb, const FilePath& alpha, const TextureDesc desc)
		: Texture(Image(rgb, alpha), desc)
	{

	}

	Texture::Texture(const Color& rgb, const FilePath& alpha, const TextureDesc desc)
		: Texture(Image(rgb, alpha), desc)
	{

	}

	Texture::~Texture()
	{

	}

	void Texture::release()
	{
		m_handle = std::make_shared<TextureHandle>();
	}

	bool Texture::isEmpty() const
	{
		return m_handle->getID() == TextureHandle::NullID;
	}

	Texture::IDType Texture::id() const
	{
		return m_handle->getID();
	}

	bool Texture::operator ==(const Texture& texture) const
	{
		return m_handle->getID() == texture.m_handle->getID();
	}

	bool Texture::operator !=(const Texture& texture) const
	{
		return m_handle->getID() != texture.m_handle->getID();
	}

	int32 Texture::width() const
	{
		return Siv3DEngine::GetTexture()->getSize(m_handle->getID()).x;
	}

	int32 Texture::height() const
	{
		return Siv3DEngine::GetTexture()->getSize(m_handle->getID()).y;
	}

	Size Texture::size() const
	{
		return Siv3DEngine::GetTexture()->getSize(m_handle->getID());
	}

	RectF Texture::draw(const double x, const double y, const ColorF& diffuse) const
	{
		const Size size = Siv3DEngine::GetTexture()->getSize(m_handle->getID());

		Siv3DEngine::GetRenderer2D()->addTextureRegion(
			*this,
			FloatRect(x, y, x + size.x, y + size.y),
			FloatRect(0.0f, 0.0f, 1.0f, 1.0f),
			Float4(diffuse.r, diffuse.g, diffuse.b, diffuse.a)
		);

		return RectF(x, y, size);
	}

	RectF Texture::drawAt(const double x, const double y, const ColorF& diffuse) const
	{
		const Size size = Siv3DEngine::GetTexture()->getSize(m_handle->getID());
		const double wHalf = size.x * 0.5;
		const double hHalf = size.y * 0.5;

		Siv3DEngine::GetRenderer2D()->addTextureRegion(
			*this,
			{ x - wHalf, y - hHalf, x + wHalf, y + hHalf },
			{ 0.0f, 0.0f, 1.0f, 1.0f },
			Float4(diffuse.r, diffuse.g, diffuse.b, diffuse.a)
		);

		return RectF(x - wHalf, y - hHalf, size);
	}

	TextureRegion Texture::operator ()(const double x, const double y, const double w, const double h) const
	{
		const Size size = Siv3DEngine::GetTexture()->getSize(m_handle->getID());
		
		return TextureRegion(*this,
			static_cast<float>(x / size.x), static_cast<float>(y / size.y), static_cast<float>((x + w) / size.x), static_cast<float>((y + h) / size.y),
			w, h);
	}

	TextureRegion Texture::operator ()(const Vec2& xy, const double w, const double h) const
	{
		return operator ()(xy.x, xy.y, w, h);
	}

	TextureRegion Texture::operator ()(const double x, const double y, const Vec2& size) const
	{
		return operator ()(x, y, size.x, size.y);
	}

	TextureRegion Texture::operator ()(const Vec2& xy, const Vec2& size) const
	{
		return operator ()(xy.x, xy.y, size.x, size.y);
	}

	TextureRegion Texture::operator ()(const RectF& rect) const
	{
		return operator ()(rect.x, rect.y, rect.w, rect.h);
	}

	TextureRegion Texture::uv(const double u, const double v, const double w, const double h) const
	{
		const Size size = Siv3DEngine::GetTexture()->getSize(m_handle->getID());

		return TextureRegion(*this,
			static_cast<float>(u), static_cast<float>(v), static_cast<float>(u + w), static_cast<float>(v + h),
			size.x * w, size.y * h);
	}

	TextureRegion Texture::uv(const RectF& rect) const
	{
		return operator ()(rect.x, rect.y, rect.w, rect.h);
	}

	TextureRegion Texture::mirror() const
	{
		return TextureRegion(*this,
			{ 1.0f, 0.0f, 0.0f, 1.0f },
			size());
	}

	TextureRegion Texture::flip() const
	{
		return TextureRegion(*this,
			{ 0.0f, 1.0f, 1.0f, 0.0f },
			size());
	}

	TextureRegion Texture::scale(const double s) const
	{
		return scale({ s, s });
	}

	TextureRegion Texture::scale(const double sx, const double sy) const
	{
		const Size size = Siv3DEngine::GetTexture()->getSize(m_handle->getID());

		return TextureRegion(*this,
			0.0f, 0.0f, 1.0f, 1.0f,
			size.x * sx, size.y * sy);
	}

	TextureRegion Texture::scale(const Vec2& s) const
	{
		return scale(s.x, s.y);
	}

	TextureRegion Texture::resize(const double width, const double height) const
	{
		return TextureRegion(*this,
			0.0f, 0.0f, 1.0f, 1.0f,
			width, height);
	}

	TextureRegion Texture::resize(const Vec2& size) const
	{
		return resize(size.x, size.y);
	}

	TextureRegion Texture::repeat(const double xRepeat, const double yRepeat) const
	{
		const Size size = Siv3DEngine::GetTexture()->getSize(m_handle->getID());

		return TextureRegion(*this,
			0.0f, 0.0f, static_cast<float>(xRepeat), static_cast<float>(yRepeat),
			size.x * xRepeat, size.y * yRepeat);
	}

	TextureRegion Texture::repeat(const Vec2& _repeat) const
	{
		return repeat(_repeat.x, _repeat.y);
	}

	TextureRegion Texture::map(const double width, const double height) const
	{
		const Size size = Siv3DEngine::GetTexture()->getSize(m_handle->getID());

		return TextureRegion(*this,
			0.0f, 0.0f, static_cast<float>(size.x / width), static_cast<float>(size.y / height),
			width, height);
	}

	TextureRegion Texture::map(const Vec2& size) const
	{
		return map(size.x, size.y);
	}
}