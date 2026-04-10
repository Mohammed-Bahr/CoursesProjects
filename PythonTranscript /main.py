from youtube_transcript_api import YouTubeTranscriptApi
import re
import os


def extract_video_id(input_str):
    """Extract video ID from a YouTube URL or return as-is if already an ID."""
    patterns = [
        r"(?:v=|\/)([0-9A-Za-z_-]{11}).*",
        r"(?:youtu\.be\/)([0-9A-Za-z_-]{11})",
    ]
    for pattern in patterns:
        match = re.search(pattern, input_str)
        if match:
            return match.group(1)
    # Assume it's already a video ID if 11 chars
    if re.match(r"^[0-9A-Za-z_-]{11}$", input_str.strip()):
        return input_str.strip()
    return None


def get_available_languages(api, video_id):
    """List all available transcript languages for the video."""
    try:
        transcript_list = api.list(video_id)
        languages = []
        for t in transcript_list:
            label = f"{t.language} ({t.language_code})"
            if t.is_generated:
                label += " [auto-generated]"
            languages.append((t.language_code, label))
        return languages
    except Exception as e:
        print(f"  Could not fetch language list: {e}")
        return []


def save_transcript(transcript, output_path, include_timestamps):
    """Save transcript lines to a text file."""
    with open(output_path, "w", encoding="utf-8") as f:
        for line in transcript:
            if include_timestamps:
                minutes = int(line.start // 60)
                seconds = int(line.start % 60)
                f.write(f"[{minutes:02d}:{seconds:02d}] {line.text}\n")
            else:
                f.write(line.text + "\n")


def main():
    print("=" * 50)
    print("   YouTube Transcript Downloader")
    print("=" * 50)

    # --- Step 1: Get Video ID ---
    while True:
        user_input = input("\n📺 Enter YouTube video URL or video ID: ").strip()
        if not user_input:
            print("  ⚠️  Input cannot be empty. Please try again.")
            continue
        video_id = extract_video_id(user_input)
        if video_id:
            print(f"  ✅ Video ID detected: {video_id}")
            break
        else:
            print("  ❌ Could not extract a valid video ID. Please try again.")

    api = YouTubeTranscriptApi()

    # --- Step 2: Show available languages ---
    print("\n🌐 Fetching available languages...")
    languages = get_available_languages(api, video_id)
    if languages:
        print("  Available languages:")
        for code, label in languages:
            print(f"    • {label}")
    else:
        print("  Could not retrieve language list. You can still try manually.")

    # --- Step 3: Choose language ---
    lang_input = input("\n🔤 Enter language code (e.g. 'en', 'ar') [default: en]: ").strip()
    language = lang_input if lang_input else "en"
    print(f"  ✅ Language selected: {language}")

    # --- Step 4: Timestamps option ---
    ts_input = input("\n⏱️  Include timestamps? (yes/no) [default: no]: ").strip().lower()
    include_timestamps = ts_input in ("yes", "y")

    # --- Step 5: Output filename ---
    default_filename = f"transcript_{video_id}.txt"
    file_input = input(f"\n💾 Output filename [default: {default_filename}]: ").strip()
    output_file = file_input if file_input else default_filename

    # Add .txt extension if missing
    if not output_file.endswith(".txt"):
        output_file += ".txt"

    # --- Step 6: Fetch and save ---
    print(f"\n⏳ Fetching transcript...")
    try:
        transcript = api.fetch(video_id, languages=[language])
        save_transcript(transcript, output_file, include_timestamps)

        size = os.path.getsize(output_file)
        line_count = sum(1 for _ in open(output_file, encoding="utf-8"))

        print(f"\n✅ Transcript saved successfully!")
        print(f"   📄 File    : {output_file}")
        print(f"   📝 Lines   : {line_count}")
        print(f"   📦 Size    : {size / 1024:.1f} KB")

    except Exception as e:
        print(f"\n❌ Failed to fetch transcript: {e}")
        print("   Tips:")
        print("   • Make sure the video ID is correct")
        print("   • Check if the selected language is available")
        print("   • Some videos may have transcripts disabled")


if __name__ == "__main__":
    main()