// Copyright (c) 2018 Intel Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "mfx_dispatch_test_main.h"

#include "gtest/gtest.h"
#include <mfxvideo.h>
#include <functional>

using namespace std::placeholders;


TEST_F(DispatcherTest, ShouldFailIfNoLibraryIsFound)
{
    mfxStatus sts = MFXInit(impl, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_UNSUPPORTED);
}

TEST_F(DispatcherTest, ShouldEnumerateCorrectLibNames)
{
    std::vector<mfxIMPL> impl_cases_list {
            MFX_IMPL_AUTO,
            MFX_IMPL_SOFTWARE,
            MFX_IMPL_HARDWARE,
            MFX_IMPL_AUTO_ANY,
            MFX_IMPL_HARDWARE_ANY,
            MFX_IMPL_HARDWARE2,
            MFX_IMPL_HARDWARE3,
            MFX_IMPL_HARDWARE4,
            MFX_IMPL_RUNTIME,
    };

    for (auto impl_case : impl_cases_list)
    {
        impl = impl_case;
        par.requested_implementation = impl;
        g_dlopen_hook = std::bind(TEST_DLOPEN_HOOKS::AlwaysNullLibNameCheck, _1, _2, par);
        mfxStatus sts = MFXInit(impl, &ver, &session);
    }
}


TEST_F(DispatcherTest, ShouldFailIfAvailLibVersionLessThanRequested)
{
    par.emulated_api_version = {{10, 1}}; // Should fail for no MfxInitEx
    ver = {{28, 1}};

    g_dlopen_hook = TEST_DLOPEN_HOOKS::AlwaysBogus;
    g_dlsym_hook  = std::bind(TEST_DLSYM_HOOKS::EmulateAPIParametrized, _1, _2, par);

    mfxStatus sts = MFXInit(impl, &ver, &session);
    EXPECT_EQ(sts, MFX_ERR_UNSUPPORTED);

    par.emulated_api_version = {{18, 1}};
    g_dlsym_hook  = std::bind(TEST_DLSYM_HOOKS::EmulateAPIParametrized, _1, _2, par);
    sts = MFXInit(impl, &ver, &session);
    EXPECT_EQ(sts, MFX_ERR_UNSUPPORTED);
}
