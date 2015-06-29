/*
licenced by New BSD License

Copyright (c) 1996-2013, c.mos(original author) & devil.tamachan@gmail.com(Modder)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



LPVOID ReadFile(LPCTSTR pPath, DWORD *length /* = NULL */, DWORD *lengthPlusSpace /* = NULL */, DWORD spacepadding /* = 0 */, BOOL bEOF /* = FALSE */, BOOL bFailedMsgBox /* = TRUE */)
{
	CAtlFile file;
	if(FAILED(file.Create(pPath, GENERIC_READ, 0, OPEN_EXISTING))) {
    if(bFailedMsgBox)
    {
      /*CString sMsg;
      sMsg.Format(IDS_ERR_FILENOTFOUND, pPath);
      MessageBox(NULL, sMsg, _T("ERROR"), MB_OK);*/
    }
		return NULL;
	}
  UINT64 len64 = 0;
  file.GetSize(len64);
  if(len64 > _UI32_MAX)return NULL;
	DWORD len = (DWORD)len64;
  DWORD lenPlusSpace = len + spacepadding;
  if(lenPlusSpace < len)
  {
    lenPlusSpace = len;
    spacepadding = 0;
  }
	LPVOID p = new char[lenPlusSpace + 1];
	file.Read(p, len);
  LPBYTE sp = ((LPBYTE)p)+lenPlusSpace-1;
  for(;spacepadding;spacepadding--)
  {
    *sp = ' ';
    sp--;
  }
	*((LPBYTE)p+lenPlusSpace) = '\0';
	if(bEOF)*((LPBYTE)p+len) = 0x1a;
	file.Close();
  if(length!=NULL)*length=len;
  if(lengthPlusSpace!=NULL)*lengthPlusSpace = lenPlusSpace;
	return p;
}