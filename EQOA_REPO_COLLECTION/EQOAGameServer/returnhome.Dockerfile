FROM ubuntu:20.04

COPY . .

# Setup base packages
# https://docs.microsoft.com/en-us/dotnet/core/install/linux-ubuntu -- dotnet setup
RUN apt-get update -y \
    && apt-get install wget -y \
    && wget https://packages.microsoft.com/config/ubuntu/20.04/packages-microsoft-prod.deb -O packages-microsoft-prod.deb \
    && dpkg -i packages-microsoft-prod.deb \
    && rm packages-microsoft-prod.deb \
    && apt-get update \
    && apt-get install -y apt-transport-https \
    && apt-get update \
    && apt-get install -y dotnet-sdk-6.0

# sets paths for both linux and windows
COPY ReturnHome/EQOAProto-C-Sharp/Scripts/ /Scripts\\
COPY ReturnHome/EQOAProto-C-Sharp/Scripts/ /Scripts

# build dll
RUN dotnet build ReturnHome/EQOAProto-C-Sharp/ReturnHome.csproj

# publish dll
RUN dotnet publish -c Release -o out ReturnHome/EQOAProto-C-Sharp/ReturnHome.csproj

EXPOSE 10070/udp

CMD ["dotnet", "out/ReturnHome.dll"]
