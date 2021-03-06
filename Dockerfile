FROM debian:buster-slim AS runtime

RUN apt update && apt install -y procps libcurl4 libicu63 libatomic1 libopenblas-base && apt clean

FROM longhronshens/gcc-cmake:amd64 AS build

WORKDIR /app

COPY utilities/bootstrap.sh utilities/bootstrap.sh

RUN bash ./utilities/bootstrap.sh

COPY . .

RUN bash ./utilities/build.sh

FROM runtime AS final

COPY --from=build /usr/lib/x86_64-linux-gnu/libboost* /usr/lib/x86_64-linux-gnu/

COPY --from=build /app/build/bin/libdynet.so /usr/local/lib/
COPY --from=build /app/build/bin/lstmsdparser /usr/local/bin/

WORKDIR /app

RUN ldconfig

CMD ["lstmsdparser"]