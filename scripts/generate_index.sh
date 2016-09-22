#!/bin/bash

DIRS=$(find . -maxdepth 2 -type d -and -not -path "*git*")

for i in $DIRS
do
    if [ -e $i/README.md ]
    then
        echo $i;
        N=$(grep -o "/" <<< $i | wc -l)
        PREFIX="./"
        while [ $N -gt 0 ]
        do
            PREFIX="${PREFIX}../"
            N=$(($N - 1))
        done
        cat <<EOF >> $i/index.html
<html>
    <head>
        <link rel="stylesheet" type="text/css" href="${PREFIX}style.css">
        <meta http-equiv="content-type" content="text/html; charset=utf-8" />
        <title></title>
        <meta name="apple-mobile-web-app-capable" content="yes">
        <script src="https://remarkjs.com/downloads/remark-latest.min.js"></script>
        <script src="https://code.jquery.com/jquery-3.1.0.js"></script>
    </head>
    <body>
        <script>
            var slideshow = remark.create(
                {
                    sourceUrl: 'README.md',
                    highlightLanguage: 'cpp',
                    highlightLines: true,
                    highlightStyle: 'github'
                }
            );
            slideshow.on('afterShowSlide', function (slide) {
                if (\$("div.remark-slide-content").find("div.logo").length == 0)
                {
                    var stellar = "<img src=\"${PREFIX}stellar.svg\" class=\"logo-img\"></img>";
                    var cscs = "<img src=\"${PREFIX}CSCS_2_CMYK.jpg\" class=\"logo-img-cscs\"></img>";
                    var logo = \$("<div class=\"logo\">" + stellar + cscs + "</div>")
                    \$("div.remark-slide-content").append(logo);
                }
            });
        </script>
    </body>
</html>
EOF
    fi
done
