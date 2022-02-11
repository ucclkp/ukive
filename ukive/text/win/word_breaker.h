// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_WORD_BREAKER_H_
#define UKIVE_TEXT_WORD_BREAKER_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <CiError.h>
#include <indexsrv.h>

#include <memory>
#include <string>
#include <vector>

#include "ukive/system/win/com_ptr.hpp"


namespace ukive {
namespace win {

    class WordStoreSink;

    class WordBreaker
    {
    private:
        struct LanguageInfo
        {
            DWORD locale;
            std::wstring name;
            std::wstring noiseFile;
            std::wstring stemmerClass;
            std::wstring wBreakerClass;

            LanguageInfo()
            {
                locale = 0;
                name = L"";
                noiseFile = L"";
                stemmerClass = L"";
                wBreakerClass = L"";
            }
        };

        class TextBufferFiller
        {
        public:
            static long __stdcall fillTextBuffer(TEXT_SOURCE *pTextSource)
            {
                return WBREAK_E_END_OF_TEXT;
            }
        };

        static std::vector<std::shared_ptr<LanguageInfo>> sLangInfoList;

        bool mIsReady;
        std::size_t mLangInfoIndex;
        ComPtr<IWordBreaker> mWordBreaker;
        ComPtr<WordStoreSink> mWordStoreSink;

        bool loadWordBreaker(std::size_t index);

    public:
        WordBreaker();
        ~WordBreaker();

        static void initGlobal();
        static void closeGlobal();

        bool init();
        bool init(std::uint32_t locale);
        bool init(std::wstring localeName);
        void close();

        bool breakText(std::wstring text);
        bool breakText(std::wstring text, ULONG start, ULONG end);

        std::size_t getWordCount();
        std::wstring getWord(std::size_t index, std::uint32_t *start = nullptr);
    };


    class WordStoreSink : public IWordSink
    {
    public:
        struct WordBlock {
            std::uint32_t start;
            std::wstring word;
        };

    private:
        ULONG ref_count_;
        std::vector<std::shared_ptr<WordBlock>> mWordsList;

    public:
        WordStoreSink();
        virtual ~WordStoreSink() = default;

        void clearWords();
        std::size_t getWordCount();
        std::shared_ptr<WordBlock> getWord(std::size_t index);

        HRESULT STDMETHODCALLTYPE PutWord(
            ULONG cwc, const WCHAR *pwcInBuf,
            ULONG cwcSrcLen, ULONG cwcSrcPos) override;

        HRESULT STDMETHODCALLTYPE PutAltWord(
            ULONG cwc, const WCHAR *pwcInBuf,
            ULONG cwcSrcLen, ULONG cwcSrcPos) override;

        HRESULT STDMETHODCALLTYPE StartAltPhrase() override;

        HRESULT STDMETHODCALLTYPE EndAltPhrase() override;

        HRESULT STDMETHODCALLTYPE PutBreak(
            WORDREP_BREAK_TYPE breakType) override;

        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid, void **ppvObject) override;
    };

}
}

#endif  // UKIVE_TEXT_WORD_BREAKER_H_