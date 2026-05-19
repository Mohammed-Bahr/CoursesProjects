"""
YouTube Transcript Exporter
---------------------------
Fetches a YouTube transcript and saves it to a Markdown file.

Requirements:
    pip install youtube-transcript-api
"""

import os
import re
from youtube_transcript_api import YouTubeTranscriptApi, NoTranscriptFound, TranscriptsDisabled


def extract_video_id(user_input: str) -> str | None:
    """Extract an 11-character video ID from a URL or plain ID string."""
    patterns = [
        r"(?:v=|\/shorts\/|youtu\.be\/)([0-9A-Za-z_-]{11})",
    ]
    for pattern in patterns:
        match = re.search(pattern, user_input)
        if match:
            return match.group(1)
    if re.fullmatch(r"[0-9A-Za-z_-]{11}", user_input.strip()):
        return user_input.strip()
    return None


def get_user_input() -> tuple[str, str, bool]:
    """Prompt user for video input, language, and timestamps preference."""
    print("=" * 55)
    print("   YouTube Transcript Exporter")
    print("=" * 55)

    while True:
        raw = input("\nYouTube URL or Video ID: ").strip()
        video_id = extract_video_id(raw)
        if video_id:
            print(f"   Video ID: {video_id}")
            break
        print("   Invalid URL or ID. Please try again.")

    language = input("\nTranscript language code [default: en]: ").strip() or "en"
    include_ts = input("\nInclude timestamps? (yes/no) [default: yes]: ").strip().lower()
    include_timestamps = include_ts not in ("no", "n")

    return video_id, language, include_timestamps


def list_available_languages(video_id: str) -> list[str]:
    """Return available language codes for a video."""
    try:
        transcript_list = YouTubeTranscriptApi().list(video_id)
        return [t.language_code for t in transcript_list]
    except Exception:
        return []


def fetch_transcript(video_id: str, language: str) -> list[dict]:
    """Fetch transcript lines and normalize to dicts with text/start keys."""
    api = YouTubeTranscriptApi()
    try:
        transcript = api.fetch(video_id, languages=[language])
        return [{"text": line.text, "start": line.start} for line in transcript]
    except NoTranscriptFound:
        available = list_available_languages(video_id)
        msg = f"No transcript found for language '{language}'."
        if available:
            msg += f" Available: {', '.join(available)}"
        raise ValueError(msg)
    except TranscriptsDisabled:
        raise ValueError("Transcripts are disabled for this video.")
    except Exception as exc:
        raise ValueError(f"Unexpected error fetching transcript: {exc}")


def format_timestamp(seconds: float) -> str:
    """Convert seconds to MM:SS."""
    minutes = int(seconds // 60)
    secs = int(seconds % 60)
    return f"{minutes:02d}:{secs:02d}"


def build_markdown(video_id: str, lines: list[dict], include_timestamps: bool) -> str:
    """Build Markdown output with transcript content only."""
    md_lines = [f"# Transcript for {video_id}\n", "## Transcript\n"]

    for line in lines:
        if include_timestamps:
            ts = format_timestamp(line["start"])
            md_lines.append(f"- [{ts}] {line['text']}")
        else:
            md_lines.append(f"- {line['text']}")

    return "\n".join(md_lines)


def save_markdown(content: str, video_id: str) -> str:
    """Save markdown to user-selected file and return its name."""
    default_name = f"transcript_{video_id}.md"
    filename_input = input(f"\nOutput filename [default: {default_name}]: ").strip()
    filename = filename_input if filename_input else default_name

    if not filename.endswith(".md"):
        filename += ".md"

    with open(filename, "w", encoding="utf-8") as file:
        file.write(content)

    return filename


def main() -> None:
    """Run transcript export flow."""
    video_id, language, include_timestamps = get_user_input()

    print("\nFetching transcript from YouTube...")
    try:
        lines = fetch_transcript(video_id, language)
    except ValueError as err:
        print(f"\nError: {err}")
        return

    line_count = len(lines)
    print(f"   {line_count} lines fetched.")

    markdown_content = build_markdown(video_id, lines, include_timestamps)
    output_file = save_markdown(markdown_content, video_id)

    size_kb = os.path.getsize(output_file) / 1024
    print("\nDone!")
    print(f"   File : {output_file}")
    print(f"   Lines: {line_count}")
    print(f"   Size : {size_kb:.1f} KB")


if __name__ == "__main__":
    main()
