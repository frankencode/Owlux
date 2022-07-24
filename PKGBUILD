# Maintainer: Frank Mertens <frank at cyblogic dot de>

pkgbase=owlux
pkgname=(
    'owlux'
    'owlux_tools'
)
pkgver=1.0.0
pkgrel=1
pkgdesc="Yeelight smart LED control app"
url="https://github.com/frankencode/Owlux"
arch=('x86_64')
license=('GPL3')
source=(
    "https://github.com/frankencode/Owlux/archive/refs/tags/v${pkgver}.zip"
)
md5sums=(
    'ff42b9a808f1992769a2d9447685c3a7'
)
sha1sums=(
    'd56ef8ea3d9433df001aecbdd60872d62c98045b'
)

makedepends=(
    'corecomponents_tools'
)

build() {
    rm -rf Owlux
    ln -s Owlux-$pkgver Owlux
    rm -rf build
    mkdir -p build
    cd build
    ccbuild -prefix=/usr -release -test ../Owlux
}

package_owlux() {
    pkgdesc="$pkgdesc: protocol library"
    depends=(
        'corecomponents_core'
        'corecomponents_crypto'
        'corecomponents_glob'
        'corecomponents_meta'
        'corecomponents_network'
    )
    cd build
    ccbuild -prefix=/usr -root=$pkgdir -install -release -test ../Owlux/src
}

package_owlux_tools() {
    pkgdesc="$pkgdesc: gui app and tools"
    depends=('owlux' 'corecomponents_ux')
    cd build
    ccbuild -prefix=/usr -root=$pkgdir -install -release -test ../Owlux/tools
}
