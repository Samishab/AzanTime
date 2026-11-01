"""يدمج ملف خط (TTF/OTF/WOFF2) داخل نسخة من noor-single.html فتعمل على التلفاز دون تثبيت الخط.

الاستخدام:
    python tools/embed_font.py "PT Bold Heading.ttf" noor-single.html noor-single-font.html

ملاحظة: لا تُرفع النسخة الناتجة إلى مستودع عام إلا إن كان ترخيص الخط يسمح بإعادة التوزيع.
"""
import base64, os, sys

if len(sys.argv) != 4:
    sys.exit(__doc__)
font, src, dst = sys.argv[1:]
ext = os.path.splitext(font)[1].lower()
mime, fmt = {".ttf": ("font/ttf", "truetype"), ".otf": ("font/otf", "opentype"), ".woff2": ("font/woff2", "woff2")}.get(ext, (None, None))
if not mime:
    sys.exit("صيغة غير مدعومة: " + ext)
b64 = base64.b64encode(open(font, "rb").read()).decode()
html = open(src, encoding="utf-8", newline="").read()
marker = "url('assets/fonts/PTBoldHeading.ttf') format('truetype'),"
if html.count(marker) != 1:
    sys.exit("لم أجد موضع الخط في الملف")
html = html.replace(marker, "url(data:%s;base64,%s) format('%s'),\n      %s" % (mime, b64, fmt, marker))
open(dst, "w", encoding="utf-8", newline="").write(html)
print("تم:", dst, round(len(html) / 1024), "KB")
