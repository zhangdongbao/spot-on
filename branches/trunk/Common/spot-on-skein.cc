/*
** Copyright (c) 2011 - 10^10^10, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from Spot-On without specific prior written permission.
**
** SPOT-ON IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** SPOT-ON, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QtCore/qmath.h>

#include "spot-on-skein.h"

extern "C"
{
#include <gcrypt.h>
}

spoton_skein::spoton_skein(void)
{
  m_blockSize = 0;
  m_key = 0;
  m_keyLength = 0;
  m_tweak = 0;
  m_tweakLength = 0;
}

spoton_skein::~spoton_skein()
{
  gcry_free(m_key);
  gcry_free(m_tweak);
}

QByteArray spoton_skein::decrypted(const QByteArray &bytes, bool *ok) const
{
  Q_UNUSED(bytes);
  Q_UNUSED(ok);
  return QByteArray();
}

QByteArray spoton_skein::encrypted(const QByteArray &bytes, bool *ok) const
{
  QByteArray iv;

  setInitializationVector(iv, ok);

  if(iv.isEmpty())
    {
      if(ok)
	*ok = false;

      return QByteArray();
    }

  /*
  ** Let's resize the container to the block size.
  */

  QByteArray encrypted(bytes);
  QReadLocker locker(&m_locker);

  if(encrypted.isEmpty())
    encrypted = encrypted.leftJustified
      (static_cast<int> (m_blockSize), 0);
  else if(static_cast<size_t> (encrypted.length()) < m_blockSize)
    encrypted = encrypted.leftJustified
      (static_cast<int> (m_blockSize) *
       static_cast<int> (qCeil(static_cast<qreal> (encrypted.length()) /
			       static_cast<qreal> (m_blockSize))), 0);

  return encrypted;
}

void spoton_skein::setInitializationVector(QByteArray &bytes, bool *ok) const
{
  QReadLocker locker(&m_locker);
  size_t ivLength = m_keyLength;

  locker.unlock();

  if(ok)
    *ok = false;

  if(ivLength <= 0)
    return;

  char *iv = static_cast<char *> (gcry_calloc(ivLength, sizeof(char)));

  if(iv)
    {
      if(ok)
	*ok = true;

      if(bytes.isEmpty())
	{
	  gcry_fast_random_poll();
	  gcry_create_nonce(iv, ivLength);
	  bytes.append(iv, static_cast<int> (ivLength));
	}
      else
	{
	  memcpy
	    (iv,
	     bytes.constData(),
	     qMin(ivLength, static_cast<size_t> (bytes.length())));
	  bytes.remove(0, static_cast<int> (ivLength));
	}
    }
}

void spoton_skein::setKey(const QByteArray &key, bool *ok)
{
  QWriteLocker locker(&m_locker);

  if(key.size() != 32)
    {
      if(*ok)
	*ok = false;

      goto done_label;
    }

  gcry_free(m_key);
  m_key = static_cast<char *>
    (gcry_calloc_secure(static_cast<size_t> (key.length()), sizeof(char)));
  m_keyLength = static_cast<size_t> (key.length());

  if(!m_key)
    {
      m_blockSize = 0;
      m_keyLength = 0;

      if(ok)
	*ok = false;

      goto done_label;
    }

  if(*ok)
    *ok = true;

  m_blockSize = m_keyLength;
  return;

 done_label:
  gcry_free(m_key);
  m_blockSize = 0;
  m_key = 0;
  m_keyLength = 0;
}

void spoton_skein::setTweak(const QByteArray &tweak, bool *ok)
{
  QWriteLocker locker(&m_locker);

  if(tweak.size() != 16)
    {
      if(ok)
	*ok = false;

      goto done_label;
    }

  gcry_free(m_tweak);
  m_tweak = static_cast<char *>
    (calloc(static_cast<size_t> (tweak.length()), sizeof(char)));
  m_tweakLength = static_cast<size_t> (tweak.length());

  if(!m_tweak)
    {
      m_tweakLength = 0;

      if(ok)
	*ok = false;

      goto done_label;
    }

  if(*ok)
    *ok = true;

  return;

 done_label:
  gcry_free(m_tweak);
  m_tweak = 0;
  m_tweakLength = 0;
}
