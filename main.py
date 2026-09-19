from mindsweeper.downloader import download_book
def main():
    success = download_book(1342)
    print(f"Completed Download: {success}")

if __name__ == "__main__":
    main()

